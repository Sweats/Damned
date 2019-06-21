#ifndef TERROR_INSANITY_SYSTEM_H
#define TERROR_INSANITY_SYSTEM_H

#include "InsanityEffect/InsanityEffect.h"

namespace terror {
	typedef PointerList<InsanityEffect> InsanityEffectPtrList;

	class InsanitySystem : public EventHandler {
	public:
		InsanitySystem(CharacterHandlerPtr characterHandler, CollisionTesterPtr collisionTester, RenderCameraPtr camera, PostProcessingEffectPtr monsterInFovView)
			: EventHandler(20), characterHandler(characterHandler), collisionTester(collisionTester), camera(camera), monsterInFovView(monsterInFovView), maxInsanity(1000.0f), 
			defaultInsanityIncreaseRate(1.0f),	maxDistanceFromHuman(10.0f), maxDistanceFromMonster(10.0f), humanDeathInsanityIncrease(60.f), lockedObjectInsanityIncrease(0.0f), 
			scareTrapInsanityIncrease(10.f), maxInsanityReached(false), monsterArmInsanityIncrease(25.0f), monsterWasInFov(false), gameSettings(GameSettings::getInstance())
		{
			this->insanity = EnergyPtr(new Energy(this->maxInsanity, this->defaultInsanityIncreaseRate, 0.0f));
		}

		void setLocalHuman(HumanPtr localHuman) {
			this->insanity->consumeAll();
			this->localHuman = localHuman;
		}

		float getInsanityPoints() {
			return this->insanity->getValue();
		}

		void notifyHumanDeath(HumanPtr human) {
			if (!this->gameSettings->isInsanityAllowed()) {
				return;
			}
			if (this->localHuman != human) {				
				this->insanity->recover(this->humanDeathInsanityIncrease);
			}else {
				this->reset();

			}
		}

		void notifyLockedObject(ObjectPtr object) {
			if (!this->gameSettings->isInsanityAllowed()) {
				return;
			}
			if (this->localHuman && this->isObjectClose(object)) {
				this->insanity->recover(this->lockedObjectInsanityIncrease);
			}
		}

		void notifyScareTrapTriggered(ScareTrapPtr scareTrap) {
			if (!this->gameSettings->isInsanityAllowed()) {
				return;
			}
			if (this->localHuman && scareTrap->isSurvivorInRange(this->localHuman)) {
				this->insanity->recover(this->scareTrapInsanityIncrease);
			}
		}

		void notifyMonsterArmContact() {
			if (!this->gameSettings->isInsanityAllowed()) {
				return;
			}
			if (this->localHuman) {
				this->insanity->recover(this->monsterArmInsanityIncrease);
			}
		}

		void notifyStageUnloaded() {
			this->reset();
		}

		void connectInsanityEffect(InsanityEffectPtr insanityEffect) {
			this->inactiveInsanityEffects.push_back(insanityEffect);
		}

		void reset() {
			this->maxInsanityReached = false;
			this->insanity->consumeAll();
			this->localHuman.reset();
			this->monsterInFovView->setEnabled(false);
			this->inactivateEffects();
			this->resetEffects();
		}

		virtual ~InsanitySystem(){}

	protected:
		GameSettingsPtr gameSettings;
		float maxInsanity;
		float defaultInsanityIncreaseRate;
		float maxDistanceFromHuman;
		float maxDistanceFromMonster;
		float humanDeathInsanityIncrease;
		float lockedObjectInsanityIncrease;
		float scareTrapInsanityIncrease;
		float monsterArmInsanityIncrease;
		EnergyPtr insanity;
		CharacterHandlerPtr characterHandler;
		HumanPtr localHuman;
		CollisionTesterPtr collisionTester;
		RenderCameraPtr camera;
		PostProcessingEffectPtr monsterInFovView;
		InsanityEffectPtrList inactiveInsanityEffects;
		InsanityEffectPtrList activeInsanityEffects;
		bool maxInsanityReached;
		bool monsterWasInFov;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->gameSettings->isInsanityAllowed()) {
				return true;
			}
			if (!this->localHuman) {
				return true;
			}
			this->insanity->setRegenerationRate(this->computeInsanityIncreaseRate());
			this->insanity->regenerate(diffTime);
			if (this->shouldActivateNextEffect()) {
				this->activateNextEffect();
			}
			this->updateEffects(diffTime);
			if (!this->maxInsanityReached && this->insanity->isMax()) {
				this->maxInsanityReached = true;
				this->reduceMaxTimeToStartEffects(20.0f);
			}
			return true;
		}

		float computeInsanityIncreaseRate() {
			float insanityIncreaseRate = this->defaultInsanityIncreaseRate;
			insanityIncreaseRate += this->computeFlashlightContribution();
			insanityIncreaseRate += this->computeOtherSurvivorsContribution();
			insanityIncreaseRate += this->computeMonsterContribution();
			return insanityIncreaseRate;
		}

		float computeFlashlightContribution() {
			if (this->localHuman->getFlashlightState()) {
				return -0.75f;
			}
			return 0.0f;
		}

		float computeOtherSurvivorsContribution() {
			float insanityIncreaseRateDiff = 0.0;
			HumanPtrMap& humans = this->characterHandler->getHumansAlive();
			foreach(HumanPtrMap::value_type entry, humans){
				if (entry.second != this->localHuman && this->humanIsCloseEnough(entry.second)) {
					insanityIncreaseRateDiff -= 0.3f;
				}
			}
			return insanityIncreaseRateDiff;
		}

		bool humanIsCloseEnough(HumanPtr human) {
			if (this->computeDistanceFromHuman(human) < this->maxDistanceFromHuman) {
				return isHumanInTheSameRoom(human);
			}
			return false;
		}

		float computeDistanceFromHuman(HumanPtr human) {
			Vector3d localHumanPosition = this->localHuman->getActor()->getPosition();
			Vector3d otherHumanPosition = human->getActor()->getPosition();
			return (localHumanPosition - otherHumanPosition).norm();
		}

		float isHumanInTheSameRoom(HumanPtr human) {
			ClosestRayResultCallback callback;
			foreach(HumanPtrMap::value_type entry, this->characterHandler->getHumansAlive()) {
				callback.addIgnoreObject(entry.second->getActor()->getCollisionObject().get());
			}
			Vector3d from = this->localHuman->getActor()->getPosition();
			Vector3d to = human->getActor()->getPosition();
			return !this->collisionTester->rayTest(toBullet(from), toBullet(to), callback);
		}

		float computeMonsterContribution() {
			MonsterPtr monster = this->characterHandler->getMonster();
			if (!monster) {
				return 0.0f;
			}
			if (monster->getType() == SPECTRUM) {
				if (!static_ptr_cast<Spectrum>(monster)->isVisible()){
					if (this->monsterWasInFov) {
						this->monsterWasInFov = false;
						this->monsterInFovView->setEnabled(false);
					}
					return 0.0f;
				}
			}
			if (monster->getType() == STATUE) {
				if (static_ptr_cast<Statue>(monster)->getState() != DESTROYER){
					if (this->monsterWasInFov) {
						this->monsterWasInFov = false;
						this->monsterInFovView->setEnabled(false);
					}
					return 0.0f;
				}
			}
			Vector3d camPosition = toAlgebra(camera->getPosition());
			Vector3d monsterPosition = monster->getActor()->getPosition();
			Vector3d camToMonster = monsterPosition - camPosition;
			float monsterDistance = camToMonster.norm();
			if (monsterDistance > this->maxDistanceFromMonster) {
				this->monsterWasInFov = false;
				this->monsterInFovView->setEnabled(false);
				return 0.0f;
			}
			camToMonster.normalize();
			Vector3d eyeDirection = toAlgebra(camera->getDirection());
			bool isInFov = dot(eyeDirection, camToMonster) > 0.9f;
			if (isInFov) {
				ClosestRayResultCallback callback;
				callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
				if (this->collisionTester->rayTest(toBullet(camPosition), toBullet(monsterPosition), callback)) {
					isInFov = false;
				}
			}
			if (isInFov) {
				if (!this->monsterWasInFov) {
					this->monsterWasInFov = true;
					if (monster->getType() != STATUE) {
						this->monsterInFovView->setEnabled(true);
					}
				}
				return 1.5f;
			}
			if (this->monsterWasInFov) {
				this->monsterWasInFov = false;
				this->monsterInFovView->setEnabled(false);
			}
			return 0.0f;
		}

		bool isObjectClose(ObjectPtr object) {
			Vector3d humanPosition = this->localHuman->getActor()->getPosition();
			Vector3d objectPosition = object->getPosition();
			float distance = (humanPosition - objectPosition).norm();
			return distance < 2.0f;
		}

		bool shouldActivateNextEffect() {
			if (this->inactiveInsanityEffects.size() == 0) {
				return false;
			}
			return this->inactiveInsanityEffects.front()->getMinimumInsanityToActivate() < this->insanity->getValue();
		}

		void activateNextEffect() {
			this->activeInsanityEffects.push_back(this->inactiveInsanityEffects.front());
			this->inactiveInsanityEffects.pop_front();
		}

		void inactivateEffects() {
			reverse_foreach(InsanityEffectPtr effect, this->activeInsanityEffects) {
				this->inactiveInsanityEffects.push_front(effect);
			}
			this->activeInsanityEffects.clear();
		}

		void resetEffects() {
			foreach(InsanityEffectPtr effect, this->activeInsanityEffects) {
				effect->reset();
			}
			foreach(InsanityEffectPtr effect, this->inactiveInsanityEffects) {
				effect->reset();
			}
		}

		void updateEffects(float diffTime) {
			foreach(InsanityEffectPtr effect, this->activeInsanityEffects) {
				effect->update(diffTime);
			}
		}

		void reduceMaxTimeToStartEffects(float timeReduction) {
			foreach(InsanityEffectPtr effect, this->activeInsanityEffects) {
				float maxTimeToStart = effect->getMaximumTimeToStart();
				maxTimeToStart -= timeReduction;
				if (maxTimeToStart < 0.0f) {
					maxTimeToStart = 0.0f;
				}
			}
		}
	};

	typedef SharedPointer<InsanitySystem> InsanitySystemPtr;	
}

#endif
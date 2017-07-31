#ifndef TERROR_MONSTER_ARMS_EFFECT_H
#define TERROR_MONSTER_ARMS_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"
#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class MonsterArmsEffect : public InsanityEffect {
	public:
		MonsterArmsEffect(CollisionTesterPtr collisionTester, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient, SoundPtr gaspingGirl, SoundPtr gaspingMan)
			: InsanityEffect(110.0f, 150.0f, 20.0f, 500.0f), collisionTester(collisionTester), characterHandler(characterHandler), terrorClient(terrorClient), gaspingGirl(gaspingGirl), 
			gaspingMan(gaspingMan), armDisapearingAnimationTriggered(false), maxNumberOfArms(20), maxSpawningAttempts(15), timeToIgnoreCollision(-1.0f)
		{			
			this->timeToMonsterArmDisapear = this->executionTime - 2.0f;
		}

		void notifyStageLoaded(StagePtr stage) {
			this->createMonsterArms();
		}

		void notifyStageUnloaded() {
			this->destroyMonsterArms();
		}

		void connectMonsterArmContactEvent(const BasicSignal::slot_type& monsterArmContactEvent) {
			this->monsterArmContactSignal.connect(monsterArmContactEvent);
		}

		virtual ~MonsterArmsEffect(){}

	protected:
		ObjectDataPtr monsterArmData;
		ObjectFactoryPtr objectFactory;
		ObjectPtrList monsterArms;
		CollisionTesterPtr collisionTester;
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		float timeToMonsterArmDisapear;
		float timeToIgnoreCollision;
		int maxNumberOfArms;
		int maxSpawningAttempts;
		bool armDisapearingAnimationTriggered;
		HumanPtr localHuman;
		int quadrant;
		Vector3d quadrantVector0;
		Vector3d quadrantVector1;
		BasicSignal monsterArmContactSignal;
		SoundPtr gaspingGirl;
		SoundPtr gaspingMan;

	private:
		virtual void doStartExecution() {
			this->localHuman = this->getLocalHuman();
			if (!this->localHuman) {
				return;	
			}

			this->showMonsterArms();
			this->moveArmsDown();
			this->setArmsOrientation();
			this->queueMonsterArmsSpawningAnimations();
			this->timeToMonsterArmDisapear = this->executionTime - 2.0f;
			this->armDisapearingAnimationTriggered = false;
			this->timeToIgnoreCollision = -1.0f;
		}

		virtual void doExecute(float diffTime) {
			this->localHuman = this->getLocalHuman();
			if (!this->localHuman) {
				return;				
			}

			if (this->timeToIgnoreCollision < 0.0f) {
				AllButMeContactResultCallback callback(this->localHuman->getActor()->getCollisionObject().get());
				this->collisionTester->contactTest(this->localHuman->getActor()->getCollisionObject(), callback);
				foreach (btCollisionObject* hit, callback.hits) {
					Object* object = static_cast<Object*>(hit->getUserPointer());
					if (object && object->getMask() == MONSTER_ARM) {
						this->localHuman->stopSprint();
						this->localHuman->consumeAllStamina();
						if (this->localHuman->getCharacterType() == CHAR_0) {
							this->gaspingGirl->play();
						}else {
							this->gaspingMan->play();
						}
						this->localHuman->setSpeedMultiplier(0.2f);
						this->timeToIgnoreCollision = 3.0f;
						this->monsterArmContactSignal();
						break;
					}
				}
			}else {
				this->timeToIgnoreCollision -= diffTime;
				if (this->timeToIgnoreCollision < 0.0f) {
					this->localHuman->setSpeedMultiplier(1.0f);
					this->timeToIgnoreCollision = -1.0f;
				}
			}


			this->timeToMonsterArmDisapear -= diffTime;
			if (!this->armDisapearingAnimationTriggered && this->timeToMonsterArmDisapear < 0.0f) {
				this->armDisapearingAnimationTriggered = true;
				this->queueMonsterArmsDisapearingAnimation();
			}
		}

		virtual void doEndExecution() {			
			if (!this->localHuman) {
				return;				
			}	
			this->localHuman->setSpeedMultiplier(1.0f);			
			this->hideMonsterArms();
		}

		virtual void doAdditionalReset() {
			this->localHuman.reset();
			this->hideMonsterArms();
		}

		void createMonsterArms() {
			for (int a = 0; a < this->maxNumberOfArms; a++) {
				ObjectPtr arm = EntitySystem::getInstance()->createObject("monster_arm");
				arm->hide();
				this->monsterArms.push_back(arm);
			}
		}

		void destroyMonsterArms() {
			foreach (ObjectPtr arm, this->monsterArms) {
				if (arm) {
					arm->destroy();
				}
			}
			this->monsterArms.clear();
		}

		HumanPtr getLocalHuman() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				throw std::runtime_error("Tried to get invalid local player.");
			}
			CharacterType thisChar = playerInfo->getCharacter();
			return this->characterHandler->getHumanAlive(thisChar);
		}

		void showMonsterArms() {
			foreach (ObjectPtr arm, this->monsterArms) {
				if (this->tryToShowMonsterArm(arm, 0)) {
					arm->show();
					arm->enablePhysics();
				}
			}
		}

		bool tryToShowMonsterArm(ObjectPtr arm, int attempt) {
			if (attempt >= this->maxSpawningAttempts) {
				return false;
			}
			Vector3d position = this->randomizePosition();
			arm->setPosition(position);
			if (this->validateArmPosition(arm)) {
				return true;
			}
			++attempt;
			return this->tryToShowMonsterArm(arm, attempt);
		}

		Vector3d randomizePosition() {
			float placeCoefficient = this->computePlaceCoefficient();
			this->findQuadrantVectors(placeCoefficient);
			Vector3d direction = this->computeDirection(placeCoefficient);
			float distance = randomNumber(5.0f, 10.0f);;
			Vector3d humanPosition = this->localHuman->getActor()->getPosition();
			return humanPosition + distance*direction - 0.7f*Vector3d::yUnit;
		}

		float computePlaceCoefficient() {
			return randomNumber(0.0f, 4.0f);
		}

		void findQuadrantVectors(float placeCoefficient) {
			this->quadrant = static_cast<int>(placeCoefficient);
			switch (this->quadrant) {
				case 0:
					this->quadrantVector0 = Vector3d::zUnit;
					this->quadrantVector1 = Vector3d::xUnit;
					break;
				case 1:
					this->quadrantVector0 = Vector3d::xUnit;
					this->quadrantVector1 = -1*Vector3d::zUnit;
					break;
				case 2:
					this->quadrantVector0 = -1*Vector3d::zUnit;
					this->quadrantVector1 = -1*Vector3d::xUnit;
					break;
				case 3:
					this->quadrantVector0 = -1*Vector3d::xUnit;
					this->quadrantVector1 = Vector3d::zUnit;
					break;
				default:
					this->quadrantVector0 = Vector3d::zUnit;
					this->quadrantVector1 = Vector3d::xUnit;
			}
		}

		Vector3d computeDirection(float placeCoefficient) {
			float interpolatorFactor = placeCoefficient - (float)this->quadrant;
			return slerp(this->quadrantVector0, this->quadrantVector1, interpolatorFactor);
		}

		bool validateArmPosition(ObjectPtr arm) {
			ClosestConvexResultCallback callback;
			callback.addIgnoreObject(this->localHuman->getActor()->getCollisionObject().get());
			callback.addIgnoreObject(arm->getCollisionObject().get());
			Vector3d from = this->localHuman->getActor()->getPosition(); 
			Vector3d to = arm->getPosition();
			return !this->collisionTester->convexSweepTest(arm->getConvexShape(), toBulletTransform(from), toBulletTransform(to), callback);
		}

		void moveArmsDown() {
			foreach (ObjectPtr arm, this->monsterArms) {
				ClosestNotMeConvexResultCallback callback(arm->getCollisionObject().get());
				Vector3d from = arm->getPosition(); 
				Vector3d to = arm->getPosition() - 50.0f*Vector3d::yUnit;
				if (this->collisionTester->convexSweepTest(arm->getConvexShape(), toBulletTransform(from), toBulletTransform(to), callback)) {
					arm->setPosition(lerp(from, to, callback.m_closestHitFraction) + 0.08*Vector3d::yUnit);
				}else {
					arm->hide();
					arm->disablePhysics();
				}
			}
		}


		void setArmsOrientation() {
			foreach (ObjectPtr arm, this->monsterArms) {
				Vector3d localHumanPosition = this->localHuman->getActor()->getPosition();
				Vector3d monsterArmPosition = arm->getPosition();
				Vector3d armToHuman = localHumanPosition - monsterArmPosition;
				armToHuman.setY(0.0f);
				armToHuman.normalize();
				Quaternion orientation(cross(Vector3d::yUnit, armToHuman), Vector3d::yUnit, armToHuman);
				arm->setOrientation(orientation);
			}
		}

		void queueMonsterArmsSpawningAnimations() {
			foreach (ObjectPtr arm, this->monsterArms) {
				AnimationPtr spawn = arm->getAnimation("spawn");
				spawn->setSpeedFactor(randomNumber(0.2f, 1.8f));
				AnimationPtr loop = arm->getAnimation("loop" + lexical_cast<string>(randomNumber(1,3)));
				arm->queueAnimation(spawn);
				arm->queueAnimation(loop);
			}
		}

		void queueMonsterArmsDisapearingAnimation() {
			foreach (ObjectPtr arm, this->monsterArms) {
				AnimationPtr disapear =arm->getAnimation("spawn");
				disapear->setSpeedFactor(randomNumber(-1.8f,-0.2f));
				arm->queueAnimation(disapear);
			}
		}

		void hideMonsterArms() {
			foreach(ObjectPtr arm, this->monsterArms) {
				arm->hide();
				arm->disablePhysics();
			}
		}
	};

	typedef SharedPointer<MonsterArmsEffect> MonsterArmsEffectPtr;
}

#endif
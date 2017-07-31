#ifndef TERROR_BERSERKER_ENERGY_REGENERATION_SYSTEM_H
#define TERROR_BERSERKER_ENERGY_REGENERATION_SYSTEM_H

#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class BerserkerEnergyRegenerationSystem : public EventHandler {
	public:
		BerserkerEnergyRegenerationSystem(CharacterHandlerPtr characterHandler, CollisionTesterPtr collisionTester)
			: EventHandler(20), characterHandler(characterHandler), defaultBerserkerEnergyRegenerationRate(0.04f), maxDistanceFromMonster(7.0f), maxHumanEnergyAbsortionRate(1.0f), 
			collisionTester(collisionTester)
		{
			energyAbsortionInterpolator = InterpolatorPtr(new ExponentialInterpolator(0.0f, this->maxHumanEnergyAbsortionRate, this->maxDistanceFromMonster, 0.5f, 2.0f));
		}

		void notifyStageLoaded(StagePtr stage) {
			MonsterPtr monster = this->characterHandler->getMonster();
			if (monster && monster->getType() == BERSERKER) {
				this->berserker = static_ptr_cast<Berserker>(monster);	
				this->berserkerEnergy = this->berserker->getEnergy();
				this->berserkerEnergy->consumeAll();
				this->berserkerEnergy->setRegenerationRate(this->defaultBerserkerEnergyRegenerationRate);
			}	
			if (stage->getName() == "Black_Lake_Woods") {
				this->setForestParameters();
			}
		}

		void notifyStageUnloaded() {
			this->berserkerEnergy.reset();
			this->berserker.reset();
			this->returnToDefaultParameters();
		}

		Connection connectMinEnergyForBerserkerEvent(const BasicSignal::slot_type& minEnergyForBerserkerEvent) {
			return this->minEnergyForBerserkerSignal.connect(minEnergyForBerserkerEvent);
		}

		Connection connectMinEnergyForTeleportingEvent(const BasicSignal::slot_type& minEnergyForTeleportingEvent) {
			return this->minEnergyForTeleportingSignal.connect(minEnergyForTeleportingEvent);
		}

		virtual ~BerserkerEnergyRegenerationSystem() {}

	protected:
		CharacterHandlerPtr characterHandler;
		BerserkerPtr berserker;
		InterpolatorPtr energyAbsortionInterpolator;
		CollisionTesterPtr collisionTester;
		BasicSignal minEnergyForBerserkerSignal;
		BasicSignal minEnergyForTeleportingSignal;
		EnergyPtr berserkerEnergy;
		float maxDistanceFromMonster;
		float defaultBerserkerEnergyRegenerationRate;
		float maxHumanEnergyAbsortionRate;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->berserker || this->berserker->isInBerserkerMode()) {
				return true;
			}
			bool minEnergyForBerserkerReached = this->berserkerEnergy->getValue() >= this->berserker->getMinEnergyForBerserkerMode();
			bool minEnergyForTeleportingReached = this->berserkerEnergy->getValue() >= this->berserker->getMinEnergyForTeleporting();
			float humansEnergyRegenerationRateContribution = this->computeHumansEnergyRegenerationRateContribution();
			this->berserkerEnergy->setRegenerationRate(this->defaultBerserkerEnergyRegenerationRate + humansEnergyRegenerationRateContribution);
			this->berserkerEnergy->regenerate(diffTime);
			if ((!minEnergyForTeleportingReached) && (this->berserkerEnergy->getValue() >= this->berserker->getMinEnergyForTeleporting())) {
				if (!this->berserker->isInBerserkerMode()) {
					this->minEnergyForTeleportingSignal();
				}
			}
			if ((!minEnergyForBerserkerReached) && (this->berserkerEnergy->getValue() >= this->berserker->getMinEnergyForBerserkerMode())) {
				if (!this->berserker->isInBerserkerMode()) {
					this->minEnergyForBerserkerSignal();
				}
			}
			return true;
		}

		float computeHumansEnergyRegenerationRateContribution() {
			float humansContribution = 0.0f;
			foreach(HumanPtrMap::value_type entry, this->characterHandler->getHumansAlive()) {
				float distance = this->computeDistanceFromHuman(entry.second);
				if (distance <= this->maxDistanceFromMonster && this->isHumanInTheSameRoom(entry.second)) {
					humansContribution += this->energyAbsortionInterpolator->interpolate(distance);
					if (!entry.second->isMonsterInRange()) {
						entry.second->notifyMonsterInRange();
					}
				}else {
					if (entry.second->isMonsterInRange()) {
						entry.second->notifyMonsterNotInRange();
					}
				}
			}
			return humansContribution;
		}

		bool isHumanInTheSameRoom(HumanPtr human) {
			ClosestRayResultCallback callback;
			callback.addIgnoreObject(this->berserker->getActor()->getCollisionObject().get());
			foreach(HumanPtrMap::value_type entry, this->characterHandler->getHumansAlive()) {
				callback.addIgnoreObject(entry.second->getActor()->getCollisionObject().get());
			}
			Vector3d from = this->berserker->getActor()->getPosition() + 1.0*Vector3d::yUnit;
			Vector3d to = human->getActor()->getPosition() + 1.0*Vector3d::yUnit;
			return !this->collisionTester->rayTest(toBullet(from), toBullet(to), callback);
		}

		float computeDistanceFromHuman(HumanPtr human) {
			Vector3d monsterPosition = this->berserker->getActor()->getPosition();
			Vector3d humanPosition = human->getActor()->getPosition();
			return (monsterPosition - humanPosition).norm();
		}

		void setForestParameters() {
			this->maxDistanceFromMonster = 8.0f;
			this->maxHumanEnergyAbsortionRate = 0.7f;
			this->energyAbsortionInterpolator->setSecondPoint(maxDistanceFromMonster, 0.2f);
		}

		void returnToDefaultParameters() {
			this->maxDistanceFromMonster = 7.0f;
			this->maxHumanEnergyAbsortionRate = 1.0f;
			this->energyAbsortionInterpolator->setSecondPoint(maxDistanceFromMonster, 0.5f);
		}
	};

	typedef SharedPointer<BerserkerEnergyRegenerationSystem> BerserkerEnergyRegenerationSystemPtr;
}

#endif
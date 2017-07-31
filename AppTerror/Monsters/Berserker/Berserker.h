#ifndef TERROR_BERSERKER_H
#define TERROR_BERSERKER_H

#include "Monsters/Monster.h"
#include "Monsters/Energy.h"

namespace terror {
	class Berserker : public Monster {
	public:
		Berserker(CharacterDataPtr characterData, CharacterActionsPtr characterActions, MonsterActionsPtr monsterActions) 
			: Monster(characterData, characterActions, monsterActions), berserkerMode(false), timeToReturnToGhostMode(-1.0f), minEnergyForBerserkerMode(8.0f), minEnergyForTeleporting(1.0f)
		{
			this->energy = EnergyPtr(new Energy(8.0f, 0.04f, 0.04f));
			this->actions->walk->setAnimationSpeed(0.64f);
			this->actions->run->setAnimationSpeed(3.0f);
			this->monsterActions->attack->setAnimationSpeedFactor(2.0f);
			this->scream = this->getActor()->getSoundSource()->createSound("berserk_scream.ogg");
			this->cry = this->getActor()->getSoundSource()->createSound("berserk_cry.ogg");
			if (this->cry) {
				this->cry->setRepeat(true);
				this->cry->play();
			}
		}

		virtual MonsterType getType() {
			return BERSERKER;
		}

		bool tryBerserker() {
			if (berserkerMode || this->energy->getValue() < this->minEnergyForBerserkerMode) {
				return false;
			}
			this->becomeBerserker();
			return true;
		}

		EnergyPtr getEnergy() {
			return this->energy;
		}

		float getMinEnergyForBerserkerMode() {
			return this->minEnergyForBerserkerMode;
		}

		float getMinEnergyForTeleporting() {
			return this->minEnergyForTeleporting;
		}

		bool isInBerserkerMode() {
			return this->berserkerMode;
		}

		void update(float diffTime) {
			if (this->timeToReturnToGhostMode < 0.0f) {
				return;
			}
			
			this->timeToReturnToGhostMode -= diffTime;
			if (this->timeToReturnToGhostMode < 0.0f) {
				this->timeToReturnToGhostMode = -1.0f;
				this->returnToOriginalState();
			}
		}

		void becomeBerserker() {
			this->energy->spend(minEnergyForBerserkerMode);
			this->berserkerMode = true;
			this->timeToReturnToGhostMode = 7.0f;
			if (this->scream) {
				this->scream->play();
			}
			if (this->cry) {
				this->cry->stop();
			}
			this->becomeBerserkerSignal();
		}

		void returnToOriginalState() {
			this->berserkerMode = false;
			if (this->cry) {
				this->cry->play();
			}
			this->becomeGhostSignal();
		}

		Connection connectBecomeBerserkerEvent(const BasicSignal::slot_type& becomeBerserkerEvent) {
			Connection connection = this->becomeBerserkerSignal.connect(becomeBerserkerEvent);
			if (this->berserkerMode) {
				this->becomeBerserkerSignal();
			}
			return connection;
		}

		Connection connectBecomeGhostEvent(const BasicSignal::slot_type& becomeGhostEvent) {
			Connection connection = this->becomeGhostSignal.connect(becomeGhostEvent);
			if (!this->berserkerMode) {
				this->becomeGhostSignal();
			}
			return connection;
		}

		virtual ~Berserker() {}

	protected:
		EnergyPtr energy;
		float maxEnergy;
		float minEnergyForBerserkerMode;
		float timeToReturnToGhostMode;
		float minEnergyForTeleporting;
		bool berserkerMode;
		BasicSignal becomeBerserkerSignal;
		BasicSignal becomeGhostSignal;
		SoundPtr scream;
		SoundPtr cry;

	private:
		void updateAdditionalInfo(CharacterStateInfoPtr stateInfo) {
			if (stateInfo->isVisible() && !this->berserkerMode) {
				this->becomeBerserker();
			}else if(!stateInfo->isVisible() && this->berserkerMode) {
				this->returnToOriginalState();
			}
		}
	};

	typedef SharedPointer<Berserker> BerserkerPtr;
}

#endif
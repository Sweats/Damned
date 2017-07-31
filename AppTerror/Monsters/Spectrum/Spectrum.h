#ifndef TERROR_SPECTRUM_H
#define TERROR_SPECTRUM_H

#include "Ghost/Ghost.h"
#include "Monsters/Monster.h"

namespace terror {
	class Spectrum : public Monster, public Ghost {
	public:
		Spectrum(CharacterDataPtr characterData, CharacterActionsPtr characterActions, MonsterActionsPtr monsterActions, 
			SoundPtr changeStateSound, bool forest) 
			: Monster(characterData, characterActions, monsterActions), Ghost(characterData->actor, changeStateSound)
		{
			this->actions->walk->setAnimationSpeed(3.5f);
			this->actions->run->setAnimationSpeed(3.5f);
			this->monsterActions->attack->setAnimationSpeedFactor(4.0f);

			if (forest) {
				ghostMultiplier = 2.5f;
				monsterMultiplier = 1.1f;
			}else {
				ghostMultiplier = 2.0f;
				monsterMultiplier = 1.0f;
			}

			this->connectBecomeInvisibleEvent(Bind(&Spectrum::setGhostSpeed, this));
			this->connectBecomeVisibleEvent(Bind(&Spectrum::setMonsterSpeed, this));
		}

		virtual MonsterType getType() {
			return SPECTRUM;
		}

		virtual ~Spectrum() {}

	protected:
		float ghostMultiplier;
		float monsterMultiplier;

	private:
		virtual void updateAdditionalInfo(CharacterStateInfoPtr stateInfo) {
			if (stateInfo->isVisible() && !this->isVisible()) {
				this->becomeVisible(true);
			}
			if (!stateInfo->isVisible() && this->isVisible()) {
				this->becomeInvisible();
			}
		}

		void setGhostSpeed() {
			this->actions->walk->setSpeedMultiplier(ghostMultiplier);
			this->actions->run->setSpeedMultiplier(ghostMultiplier);
		}

		void setMonsterSpeed() {
			this->actions->walk->setSpeedMultiplier(monsterMultiplier);
			this->actions->run->setSpeedMultiplier(monsterMultiplier);
		}
	};

	typedef SharedPointer<Spectrum> SpectrumPtr;
}

#endif
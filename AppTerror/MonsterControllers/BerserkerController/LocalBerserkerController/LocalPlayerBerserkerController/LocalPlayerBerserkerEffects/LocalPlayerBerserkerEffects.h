#ifndef TERROR_LOCAL_PLAYER_BERSERKER_EFFECTS_H
#define TERROR_LOCAL_PLAYER_BERSERKER_EFFECTS_H

#include "BerserkerGhostViewLogic/BerserkerGhostViewLogic.h"

namespace terror {
	class LocalPlayerBerserkerEffects{
	public:
		LocalPlayerBerserkerEffects(LightPtr monsterLight, PostProcessingEffectPtr berserkerView, PostProcessingEffectPtr ghostView, BerserkerGhostViewLogicPtr berserkerGhostViewLogic, SoundPtr berserkerMusic, 
			SoundPtr ghostMusic, EnergyPtr berserkerEnergy, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: monsterLight(monsterLight), berserkerView(berserkerView), ghostView(ghostView), berserkerGhostViewLogic(berserkerGhostViewLogic), berserkerMusic(berserkerMusic), ghostMusic(ghostMusic),
			berserkerEnergy(berserkerEnergy), terrorSave(terrorSave), buttonTranslator(buttonTranslator) {}

		void becomeBerserkerEffects() {
			this->monsterLight->setVisible(true);
			this->berserkerView->setEnabled(true);
			this->ghostView->setEnabled(false);
			if (this->ghostMusic) {
				this->ghostMusic->stop();
			}
			if (this->berserkerMusic) {
				this->berserkerMusic->play();
			}
		}

		void returnToGhostModeEffects() {
			this->monsterLight->setVisible(false);
			this->berserkerView->setEnabled(false);
			this->berserkerGhostViewLogic->setEnergy(this->berserkerEnergy->getValue());
			this->berserkerGhostViewLogic->setEnergyRegenerationRate(this->berserkerEnergy->getRegenerationRate());
			this->ghostView->setEnabled(true);
			if (this->berserkerMusic) {
				this->berserkerMusic->stop();
			}
			if (this->ghostMusic) {
				this->ghostMusic->play();
			}
		}

		void minEnergyForBerserkerEffects() {
			writeMessage("Click " + this->buttonTranslator->translateButton(this->terrorSave->controls->transform) + " to enter frenzied mode");
		}

		void minEnergyForTeleportingEffects() {
			writeMessage("Click " + this->buttonTranslator->translateButton(this->terrorSave->controls->teleport) + " to teleport");
		}

		void update() {
			this->berserkerGhostViewLogic->setEnergy(this->berserkerEnergy->getValue());
			this->berserkerGhostViewLogic->setEnergyRegenerationRate(this->berserkerEnergy->getRegenerationRate());
		}

		void resetAllEffects() {
			this->berserkerView->setEnabled(false);
			this->ghostView->setEnabled(false);
			this->berserkerGhostViewLogic->setEnergy(0.0f);
			if (this->berserkerMusic) {
				this->berserkerMusic->stop();
			}
			if (this->ghostMusic) {
				this->ghostMusic->stop();
			}
		}	

		virtual ~LocalPlayerBerserkerEffects() {}

	protected:
		LightPtr monsterLight;
		PostProcessingEffectPtr berserkerView;
		PostProcessingEffectPtr ghostView;
		BerserkerGhostViewLogicPtr berserkerGhostViewLogic;
		SoundPtr berserkerMusic;
		SoundPtr ghostMusic;
		EnergyPtr berserkerEnergy;
		TerrorSavePtr terrorSave;
		ButtonTranslatorPtr buttonTranslator;
	};

	typedef SharedPointer<LocalPlayerBerserkerEffects> LocalPlayerBerserkerEffectsPtr;
}

#endif
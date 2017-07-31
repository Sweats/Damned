#ifndef TERROR_LOCAL_PLAYER_SPECTRUM_EFFECTS_H
#define TERROR_LOCAL_PLAYER_SPECTRUM_EFFECTS_H


#include "Config.h"

namespace terror {
	typedef std::vector<SoundPtr> SoundStreamPtrVector;
	class LocalPlayerSpectrumEffects {
	public:
		LocalPlayerSpectrumEffects(LightPtr monsterLight,PostProcessingEffectPtr monsterView, PostProcessingEffectPtr ghostView, PostProcessingEffectPtr energyMaxView, 
			SoundPtr monsterMusic, SoundPtr ghostMusic, SoundStreamPtrVector becomeVisibleSounds)
			: monsterLight(monsterLight), monsterView(monsterView), ghostView(ghostView), energyMaxView(energyMaxView), monsterMusic(monsterMusic), ghostMusic(ghostMusic),
			becomeVisibleSounds(becomeVisibleSounds) {}

		void becomeInvisibleEffects() {
			this->monsterLight->setVisible(false);
			this->monsterView->setEnabled(false);
			this->ghostView->setEnabled(true);
			this->monsterMusic->stop();
			this->ghostMusic->play();
		}

		void becomeVisibleEffects(bool playSound = true) {
			this->ghostView->setEnabled(false);
			this->energyMaxView->setEnabled(false);
			this->monsterView->setEnabled(true);
			this->monsterLight->setVisible(true);
			this->ghostMusic->stop();
			this->monsterMusic->play();
			if (playSound) {
				SoundPtr becomeVisibleSound = this->becomeVisibleSounds[randomNumber(0, this->becomeVisibleSounds.size() - 1)];
				becomeVisibleSound->stop();
				becomeVisibleSound->play();
			}
		}

		void energyMaxEffects() {
			this->ghostView->setEnabled(false);
			this->energyMaxView->setEnabled(true);
			writeMessage("You may transform into physical form");
		}

		void resetAllEffects() { 
			this->ghostView->setEnabled(false);
			this->energyMaxView->setEnabled(false);
			this->monsterView->setEnabled(false);
			this->monsterMusic->stop();
			this->ghostMusic->stop();
		}

		virtual ~LocalPlayerSpectrumEffects() {}

	protected:
		LightPtr monsterLight;
		PostProcessingEffectPtr monsterView;
		PostProcessingEffectPtr ghostView;
		PostProcessingEffectPtr energyMaxView;
		SoundPtr monsterMusic;
		SoundPtr ghostMusic;
		SoundStreamPtrVector becomeVisibleSounds;
	};
	typedef SharedPointer<LocalPlayerSpectrumEffects> LocalPlayerSpectrumEffectsPtr;
}

#endif
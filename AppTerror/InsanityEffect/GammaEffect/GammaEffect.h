#ifndef TERROR_GAMMA_EFFECT_H
#define TERROR_GAMMA_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"

namespace terror {
	class GammaEffect : public InsanityEffect {
	public:
		GammaEffect(DeferredShadingSystemPtr deferredShadingSystem, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave, SoundPtr maleGasping, SoundPtr femaleGasping)
			: InsanityEffect(60.0f, 100.0f, 10.0f, 300.0f), deferredShadingSystem(deferredShadingSystem), terrorClient(terrorClient), terrorSave(terrorSave), maleGasping(maleGasping), femaleGasping(femaleGasping)
		{
			this->maleGasping->setRepeat(false);
			this->femaleGasping->setRepeat(false);
		}

		virtual ~GammaEffect(){}

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		algebra::FloatInterpolator interpolator;
		float timeToSetInterpolator;
		SoundPtr maleGasping;
		SoundPtr femaleGasping;
		bool femaleCharacter;

	private:
		virtual void doStartExecution() {
			this->interpolator.setData(this->terrorSave->gamma, 0.5f, 0.1f);
			this->timeToSetInterpolator = this->executionTime - 0.2f;
			this->checkCharacterSex();
			if (this->femaleCharacter) {
				this->femaleGasping->play();
			}else {
				this->maleGasping->play();
			}
		}

		virtual void doExecute(float diffTime) {
			if (this->interpolator.needUpdate()) {
				float gamma = this->interpolator.update(diffTime);
				this->deferredShadingSystem->setGamma(gamma);
			}
			if (this->timeToSetInterpolator < 0.0f) {
				return;
			}
			this->timeToSetInterpolator -= diffTime;
			if (this->timeToSetInterpolator < 0.0f) {
				this->interpolator.setData(0.5f, this->terrorSave->gamma, 0.1f);
			}
		}

		virtual void doEndExecution() {
			if (this->femaleCharacter) {
				this->femaleGasping->stop();
			}else {
				this->maleGasping->stop();
			}
		}

		virtual void doAdditionalReset() {
			this->doEndExecution();
			this->deferredShadingSystem->setGamma(this->terrorSave->gamma);
			this->interpolator.reset();
		}

		void checkCharacterSex() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}
			CharacterType thisChar = playerInfo->getCharacter();
			this->femaleCharacter = thisChar == CHAR_0;
		}

	};

	typedef SharedPointer<GammaEffect> GammaEffectPtr;
}

#endif
#ifndef TERROR_SOUND_TURNED_DOWN_EFFECT_H
#define TERROR_SOUND_TURNED_DOWN_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"

namespace terror {
	class SoundTurnedDownEffect : public InsanityEffect {
	public:
		SoundTurnedDownEffect(SoundPtr beep, TerrorSavePtr terrorSave)
			: InsanityEffect(60.0f, 100.0f, 4.0f, 200.0f), beep(beep), terrorSave(terrorSave)
		{
			this->beep->setVolume(this->getRealVolume(terrorSave->masterVolume));
			this->beep->setRepeat(true);
		}

		virtual ~SoundTurnedDownEffect(){}

	protected:
		SoundPtr beep;
		TerrorSavePtr terrorSave;
		FloatInterpolator interpolator;
		float timeToSetInterpolator;

	private:
		virtual void doStartExecution() {
			this->interpolator.setData(1.0f, 0.0f, 1.0f);
			this->timeToSetInterpolator = this->executionTime - 1.1f;
			this->beep->play();
		}

		virtual void doExecute(float diffTime) {
			if (this->interpolator.needUpdate()) {
				float factor = this->interpolator.update(diffTime);
				float realVolume = this->getRealVolume(factor*this->terrorSave->masterVolume)*this->getRealVolume(factor*this->terrorSave->gameVolume);
				Core::getInstance()->setMasterVolume(realVolume);	
			}
			if (this->timeToSetInterpolator < 0.0f) {
				return;
			}
			this->timeToSetInterpolator -= diffTime;
			if (this->timeToSetInterpolator < 0.0f) {
				this->interpolator.setData(0.0f, 1.0f, 1.0f);
			}
		}

		virtual void doEndExecution() {
			float realVolume = this->getRealVolume(this->terrorSave->masterVolume)*this->getRealVolume(this->terrorSave->gameVolume);
			Core::getInstance()->setMasterVolume(realVolume);	
			this->beep->stop();
		}

		virtual void doAdditionalReset() {
			this->doEndExecution();
			this->interpolator.reset();
		}

		float getRealVolume(float volume) {
			return (std::exp(2.0f*0.01f*volume) - 1.0f)/(std::exp(2.0f) - 1.0f);
		}
	};

	typedef SharedPointer<SoundTurnedDownEffect> SoundTurnedDownEffectPtr;
}

#endif
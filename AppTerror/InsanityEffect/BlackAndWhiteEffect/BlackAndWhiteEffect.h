#ifndef TERROR_BLACK_AND_WHITE_EFFECT_H
#define TERROR_BLACK_AND_WHITE_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"

namespace terror {
	class BlackAndWhiteEffect : public InsanityEffect {
	public:
		BlackAndWhiteEffect(PostProcessingEffectPtr blackAndWhiteView, SoundPtr breathSound)
			: InsanityEffect(50.0f, 90.0f, 12.0f, 700.0f), blackAndWhiteView(blackAndWhiteView), breathSound(breathSound) 
		{
			this->breathSound->setRepeat(true);
		}

		virtual ~BlackAndWhiteEffect(){}

	protected:
		PostProcessingEffectPtr blackAndWhiteView;
		SoundPtr breathSound;

	private:
		virtual void doStartExecution() {
			this->blackAndWhiteView->setEnabled(true);
			this->breathSound->play();
		}

		virtual void doExecute(float diffTime) {}

		virtual void doEndExecution() {
			this->blackAndWhiteView->setEnabled(false);
			this->breathSound->stop();		
		}

		virtual void doAdditionalReset(){
			this->doEndExecution();
		}
	};

	typedef SharedPointer<BlackAndWhiteEffect> BlackAndWhiteEffectPtr;
}

#endif
#ifndef TERROR_POSSESSED_CHARACTER_EFFECT_H
#define TERROR_POSSESSED_CHARACTER_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"

namespace terror {
	class PossessedCharacterEffect : public InsanityEffect {
	public:
		PossessedCharacterEffect(PostProcessingEffectPtr* possessedCharViews, SoundPtr interferenceSound)
			: InsanityEffect(120.0f, 160.0f, 0.2f, 900.0f), interferenceSound(interferenceSound), characterId(0) 
		{
			this->possessedCharViews = possessedCharViews;
			this->interferenceSound->setRepeat(true);
		}

		virtual ~PossessedCharacterEffect(){}

	protected:
		int characterId;
		PostProcessingEffectPtr* possessedCharViews;
		SoundPtr interferenceSound;

	private:
		virtual void doStartExecution() {
			this->characterId = randomNumber(0,3);
			this->possessedCharViews[characterId]->setEnabled(true);
			this->interferenceSound->play();
		}

		virtual void doExecute(float diffTime) {}

		virtual void doEndExecution() {
			this->possessedCharViews[characterId]->setEnabled(false);
			this->interferenceSound->stop();
		}

		virtual void doAdditionalReset() {
			this->doEndExecution();
		}
	};

	typedef SharedPointer<PossessedCharacterEffect> PossessedCharacterEffectPtr;
}

#endif
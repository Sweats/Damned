#ifndef TERROR_BLACK_AND_WHITE_EFFECT_H
#define TERROR_BLACK_AND_WHITE_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"
#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class BlackAndWhiteEffect : public InsanityEffect {
	public:
		BlackAndWhiteEffect(PostProcessingEffectPtr blackAndWhiteView, SoundPtr breathSound, CharacterHandlerPtr characterHandler)
			: InsanityEffect(50.0f, 90.0f, 12.0f, 700.0f), blackAndWhiteView(blackAndWhiteView), breathSound(breathSound), characterHandler(characterHandler)
		{
			this->breathSound->setRepeat(true);
		}

		virtual ~BlackAndWhiteEffect(){}

	protected:
		PostProcessingEffectPtr blackAndWhiteView;
		SoundPtr breathSound;
		CharacterHandlerPtr characterHandler;

	private:
		virtual void doStartExecution() {
			this->blackAndWhiteView->setEnabled(true);
			this->breathSound->play();
			/*
			//for future update
			MonsterPtr monster = this->characterHandler->getMonster();
			if (monster && monster->getType() == BERSERKER) {
				std::cout << "\nMonster is Mary";
			}
			if (monster && monster->getType() == STATUE) {
				std::cout << "\nMonster is Fallen";
			}
			if (monster && monster->getType() == BLIND) {
				std::cout << "\nMonster is Phantom";
			}
			if (monster && monster->getType() == SPECTRUM) {
				std::cout << "\nMonster is Lurker";
			}*/
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
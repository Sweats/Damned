#ifndef TERROR_FLASHLIGHT_FLASHING_EFFECT_H
#define TERROR_FLASHLIGHT_FLASHING_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"
#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class FlashlightFlashingEffect : public InsanityEffect {
	public:
		FlashlightFlashingEffect(SoundPtr flashlightSwitchSound, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient)
			: InsanityEffect(70.0f, 110.0f, 12.0f, 400.0f), flashlightSwitchSound(flashlightSwitchSound), characterHandler(characterHandler), terrorClient(terrorClient) {}

		virtual ~FlashlightFlashingEffect(){}

	protected:
		SoundPtr flashlightSwitchSound;
		TerrorClientPtr terrorClient;
		CharacterHandlerPtr characterHandler;
		HumanPtr localHuman;
		float timeToPlayFlashlightSwitch;
		float timeToSwitchState;

	private:
		virtual void doStartExecution() {
			this->setLocalHuman();
			this->timeToPlayFlashlightSwitch = this->executionTime/2.8f;
			this->computeNextTimeToSwitchState();
		}

		virtual void doExecute(float diffTime) {
			if (!this->localHuman) {
				return;
			}
			this->timeToPlayFlashlightSwitch -= diffTime;
			if (this->timeToPlayFlashlightSwitch < 0.0f) {
				if (this->flashlightSwitchSound) {
					this->flashlightSwitchSound->play();
				}
				this->timeToPlayFlashlightSwitch = this->executionTime/2.8f;
			}
			this->timeToSwitchState -= diffTime;
			if (this->timeToSwitchState < 0.0f) {
				this->localHuman->switchFlashlight(false, false);
				this->computeNextTimeToSwitchState();
			}
		}

		virtual void doEndExecution() {
			if (this->localHuman) {
				this->localHuman->setFlashlightState(localHuman->getFlashlightState(), true);
			}
		}

		virtual void doAdditionalReset() {
			this->localHuman.reset();
		}

		void computeNextTimeToSwitchState() {
			this->timeToSwitchState = randomNumber(0.02f, 0.18f);
		}

		void setLocalHuman() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}
			CharacterType thisChar = playerInfo->getCharacter();
			this->localHuman = this->characterHandler->getHumanAlive(thisChar);
		}
	};

	typedef SharedPointer<FlashlightFlashingEffect> FlashlightFlashingEffectPtr;
}

#endif
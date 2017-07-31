#ifndef TERROR_BUTTON_DISABLED_EFFECT_H
#define TERROR_BUTTON_DISABLED_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"
#include "GameStateHandler/GameStateHandler.h"

namespace terror {
	enum DisabledButton {
		MOVE_FORWARD,
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		RUN,
		CROUCH,
		RIGHT_CLICK,
		NONE,
	};

	class ButtonDisabledEffect : public InsanityEffect {
	public:
		ButtonDisabledEffect(GameStateHandlerPtr gameStateHandler, SoundPtr monsterSound, ControlsPtr controls)
			: InsanityEffect(70.0f, 110.0f, 15.0f, 600.0f), gameStateHandler(gameStateHandler), monsterSound(monsterSound), controls(controls), disabledButton(NONE), disabledButtonKeyCode(BC_UNASSIGNED) {}

		virtual ~ButtonDisabledEffect(){}

	protected:
		GameStateHandlerPtr gameStateHandler;
		SoundPtr monsterSound;
		ControlsPtr controls;
		DisabledButton disabledButton;
		ButtonCode disabledButtonKeyCode;

	private:
		virtual void doStartExecution() {
			this->monsterSound->play();
			this->disableAButton();
		}

		virtual void doExecute(float diffTime) {}

		virtual void doEndExecution() {
			this->enableDeactivatedButton();
		}

		virtual void doAdditionalReset() {
			if (this->disabledButton != NONE) {
				this->enableDeactivatedButton();
			}
		}

		void disableAButton() {
			LocalPlayerHumanControllerPtr localPlayerHumanController = this->gameStateHandler->getLocalHumanController();
			if (!localPlayerHumanController) {
				return;
			}

			this->disabledButton = (DisabledButton)randomNumber(0, 6);
			switch (disabledButton) {
				case MOVE_FORWARD:
					this->disabledButtonKeyCode = this->controls->moveForward;
					this->controls->moveForward = BC_UNASSIGNED;
					break;
				case MOVE_BACK:
					this->disabledButtonKeyCode = this->controls->moveBack;
					this->controls->moveBack = BC_UNASSIGNED;
					break;
				case MOVE_LEFT:
					this->disabledButtonKeyCode = this->controls->moveLeft;
					this->controls->moveLeft = BC_UNASSIGNED;
					break;
				case MOVE_RIGHT:
					this->disabledButtonKeyCode = this->controls->moveRight;
					this->controls->moveRight = BC_UNASSIGNED;
					break;
				case RUN:
					this->disabledButtonKeyCode = this->controls->run;
					this->controls->run = BC_UNASSIGNED;
					break;
				case CROUCH:
					this->disabledButtonKeyCode = this->controls->crouch;
					this->controls->crouch = BC_UNASSIGNED;
					break;
				case RIGHT_CLICK:
					localPlayerHumanController->lockMouseRightButton();
					break;
			}
		}

		void enableDeactivatedButton() {
			LocalPlayerHumanControllerPtr localPlayerHumanController = this->gameStateHandler->getLocalHumanController();
			if (!localPlayerHumanController) {
				return;
			}

			if (this->disabledButton == NONE) {
				return;
			}

			switch (disabledButton) {
				case MOVE_FORWARD:
					this->controls->moveForward = this->disabledButtonKeyCode;
					break;
				case MOVE_BACK:
					this->controls->moveBack = this->disabledButtonKeyCode;
					break;
				case MOVE_LEFT:
					this->controls->moveLeft = this->disabledButtonKeyCode;
					break;
				case MOVE_RIGHT:
					this->controls->moveRight = this->disabledButtonKeyCode;
					break;
				case RUN:
					this->controls->run = this->disabledButtonKeyCode;
					break;
				case CROUCH:
					this->controls->crouch = this->disabledButtonKeyCode;
					break;
				case RIGHT_CLICK:
					localPlayerHumanController->unlockMouseRightButton();
					break;
			}

			this->disabledButton = NONE;
		}
	};

	typedef SharedPointer<ButtonDisabledEffect> ButtonDisabledEffectPtr;
}

#endif
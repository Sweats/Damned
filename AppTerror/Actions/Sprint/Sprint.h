#ifndef TERROR_SPRINT_H
#define TERROR_SPRINT_H

#include "Stamina/Stamina.h"

namespace terror {
	class Sprint : public Action {
	public: 
		Sprint(CharacterDataPtr characterData, const string& animationName, const string& diagonalRightName, const string& diagonalLeftName,
			float speed, SoundPtr gasping) 
			: Action(characterData, animationName), diagonalRight(characterData->actor->getAnimation(diagonalRightName)), 
			diagonalLeft(characterData->actor->getAnimation(diagonalLeftName)), stamina(new Stamina(8.0f, 1.0f/3.0f, 1.0f, 2.0f)), 
			speed(speed), sprinting(false), leftMove(false), rightMove(false), gasping(gasping) {}

		virtual void execute() {
			this->sprinting = true;
			Vector3d velocity = this->speed*this->direction;
			this->characterData->actor->setMovementDirection(velocity);
			if (this->characterData->falling) {
				return;
			}
			
			if (this->leftMove && this->diagonalLeft) {
				this->characterData->actor->queueAnimation(this->diagonalLeft);
				return;
			}
			if (this->rightMove && this->diagonalRight) {
				this->characterData->actor->queueAnimation(this->diagonalRight);
				return;
			}
			this->characterData->actor->queueAnimation(this->animation);
		}

		bool canExecute() {
			if (!this->sprinting && !this->stamina->isSufficient() || this->characterData->crouched) {
				return false;
			}
			return true;
		}

		void update(float diffTime) {
			if (!this->sprinting) {
				this->stamina->regenerate(diffTime);
				return;
			}
			this->stamina->consume(diffTime);
			if (this->characterData->actor->isVisible() && this->stamina->isNull()) {
				if (this->gasping) {
					this->gasping->play();
				}
				this->stop();
			}
		}

		void stop() {
			this->sprinting = false;
		}

		void setMovement(float left, float front) {
			this->leftMove = false;
			if (left > 1.0e-4f) {
				this->leftMove = true;
			}

			this->rightMove = false;
			if (left < -1.0e-4f) {
				this->rightMove = true;
			}

			float multiplier = 1.0f;
			if (fabs(left) > 1.0e-4f) {
				multiplier *= fabs(left);
				left /= fabs(left);
			}
			if (fabs(front) > 1.0e-4f) {
				multiplier *= fabs(front);
				front /= fabs(front);
			}
			this->direction = left*this->characterData->actor->getLeft() + front*this->characterData->actor->getFront();
			this->direction.normalize();
			this->direction *= multiplier;
		}

		void setAnimationSpeed(float animationSpeed) {
			this->animation->setSpeedFactor(this->speed/animationSpeed);
			this->diagonalRight->setSpeedFactor(this->speed/animationSpeed);
			this->diagonalLeft->setSpeedFactor(this->speed/animationSpeed);
		}

		bool isSprinting() {
			return this->sprinting;
		}

		void setStamina(float stamina) {
			this->stamina->setValue(stamina);
		}

		virtual ~Sprint() {}

	protected:
		AnimationPtr diagonalRight;
		AnimationPtr diagonalLeft;
		StaminaPtr stamina;
		Vector3d direction;
		float speed;
		bool sprinting;
		bool leftMove;
		bool rightMove;
		SoundPtr gasping;
	};

	typedef SharedPointer<Sprint> SprintPtr;

	typedef PointerList<Sprint> SprintPtrList;

	class SprintSystem : public EventHandler {
	public:
		SprintSystem() : EventHandler(30) {}

		void add(SprintPtr sprint) {
			this->sprintActions.add(sprint);
		}

		virtual ~SprintSystem() {}

	protected:
		SprintPtrList sprintActions;

	private:
		virtual bool handleEvent(float diffTime) {
			foreach (SprintPtr sprint, this->sprintActions) {			
				sprint->update(diffTime);
			}
			return true;
		}
	};

	typedef SharedPointer<SprintSystem> SprintSystemPtr;
}

#endif
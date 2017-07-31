#ifndef TERROR_LOCAL_PLAYER_HUMAN_CONTROLLER_H
#define TERROR_LOCAL_PLAYER_HUMAN_CONTROLLER_H

#include "Monsters/Statue/Statue.h"
#include "LocalPlayerHumanEffects/LocalPlayerHumanEffects.h"
#include "HumanController/HumanController.h"
#include "ManipulationSystem/ManipulationSystem.h"
#include "SpectatorSystem/SpectatorSystem.h"
#include "ItemSystem/ItemSystem.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	class StatueTimeToUpdateHandler {
	public:
		StatueTimeToUpdateHandler(CharacterHandlerPtr characterHandler, HumanPtr localHuman) : characterHandler(characterHandler), localHuman(localHuman){}

		void update() {
			MonsterPtr monster = this->characterHandler->getMonster();
			if (!monster || monster->getType() != STATUE) {
				return;
			}

			float maxTime = 2.0f;
			float maxDistance = 20.0f;

			Vector3d humanToStatue = monster->getActor()->getPosition() - this->localHuman->getActor()->getPosition();
			float distance = humanToStatue.horizontalNorm();
			float timeToUpdate = maxTime;
			if (distance < maxDistance) {			
				timeToUpdate = maxTime*(exp(distance/maxDistance) - 1.0f)/(e - 1);
			}
			static_ptr_cast<Statue>(monster)->setTimeToUpdate(timeToUpdate);
		}

		virtual ~StatueTimeToUpdateHandler() {}

	private:
		CharacterHandlerPtr characterHandler;
		HumanPtr localHuman;
	};
	typedef SharedPointer<StatueTimeToUpdateHandler> StatueTimeToUpdateHandlerPtr;

	class LocalPlayerHumanController : public HumanController, public InputHandler, public EventHandler {
	public:
		LocalPlayerHumanController(HumanPtr human, ItemSystemPtr itemSystem, ManipulationSystemPtr manipulationSystem, SpectatorSystemPtr spectatorSystem,
			RenderCameraPtr camera, PostProcessingEffectPtr humanView, LocalPlayerHumanEffectsPtr localPlayerHumanEffects, StatueTimeToUpdateHandlerPtr statueTimeToUpdateHandler, ControlsPtr controls) 
			: HumanController(human), EventHandler(60), humanInfo(new HumanInfo(human->getCharacterType())), itemSystem(itemSystem), 
			manipulationSystem(manipulationSystem), spectatorSystem(spectatorSystem), camera(camera), humanView(humanView), localPlayerHumanEffects(localPlayerHumanEffects), 
			statueTimeToUpdateHandler(statueTimeToUpdateHandler), controls(controls), sprintKeyHit(false), walkKeyHold(false), mouseLeftButtonLocked(false), mouseRightButtonLocked(false)
		{
			camera->setOrientation(Ogre::Quaternion::IDENTITY);
			ActorPtr actor = this->human->getActor();

			Vector3d offset(0.0f, 0.0f, 0.05f);
			BonePtr headBone = actor->getBone("bn_camera");
			this->cameraPositioner = CameraPositionerPtr(new BoneCameraPositioner(camera, actor, offset, headBone));

			CameraRotatorPtr cameraRotator(new CameraRotator(camera, actor, 80.0f));
			this->cameraController = CameraControllerPtr(new CameraController(cameraRotator, 0.005f, 1.0f));
			this->cameraController->setMouse(this->mouse);
			this->cameraController->setJoysticks(this->joysticks);

			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			/*this->cameraConnection = 
				mouseCursor->connectSensibilityChangedEvent(Bind(&CameraController::setMouseSensibility, this->cameraController, _1));*/

			this->humanView->setEnabled(true);
			this->spectatorConnection = this->spectatorSystem->connectStartedEvent(Bind(&LocalPlayerHumanController::destroyCameraPositioner, this));			
			this->spectatorSystem->disable();			
		}

		virtual HumanControllerType getType() {
			return LOCAL_HUMAN_CONTROLLER;
		}

		virtual void update(HumanInfoPtr humanInfo) {}

		virtual void destroy(bool destroyHuman = true) {
			if (destroyHuman) {
				this->human->destroy();
			}
			this->humanView->setEnabled(false);
			this->localPlayerHumanEffects->reset();
			this->spectatorConnection.disconnect();
			this->cameraConnection.disconnect();
		}	

		HumanInfoPtr getHumanInfo() {
			ActorPtr actor = this->human->getActor();

			this->humanInfo->stateInfo->setTransform(actor->getPosition(), actor->getOrientation());
			this->humanInfo->setEyeDirection(this->human->getEyeDirection());
			this->humanInfo->flashlightState = this->human->getFlashlightState();
			this->humanInfo->stateInfo->setCrouched(this->human->isCrouched());
			this->humanInfo->flashlightBatteryCharge = this->human->getFlashlightBatteryCharge();
			this->humanInfo->flashlightIntensityFactor = this->human->getFlashlightIntensityFactor();

			return this->humanInfo;
		}

		void lockMouseLeftButton() {
			this->mouseLeftButtonLocked = true;
		}

		void unlockMouseLeftButton() {
			this->mouseLeftButtonLocked = false;
		}

		void lockMouseRightButton() {
			this->mouseRightButtonLocked = true;
		}

		void unlockMouseRightButton() {
			this->mouseRightButtonLocked = false;
		}

		virtual ~LocalPlayerHumanController() {}

	protected:
		ItemSystemPtr itemSystem;
		ManipulationSystemPtr manipulationSystem;
		SpectatorSystemPtr spectatorSystem;
		Connection spectatorConnection;

		HumanInfoPtr humanInfo;
		CameraPositionerPtr cameraPositioner;
		CameraControllerPtr cameraController;
		Connection cameraConnection;
		RenderCameraPtr camera;

		PostProcessingEffectPtr humanView;
		LocalPlayerHumanEffectsPtr localPlayerHumanEffects;
		StatueTimeToUpdateHandlerPtr statueTimeToUpdateHandler;

		ControlsPtr controls;
		bool sprintKeyHit;
		bool walkKeyHold;	

		bool mouseLeftButtonLocked;
		bool mouseRightButtonLocked;

	private:
		virtual bool handleInput(float diffTime) {
			if (this->human->isDead()) {
				this->localPlayerHumanEffects->playDieEffects();
				this->localPlayerHumanEffects->updateDieEffects(diffTime);

				GameSettingsPtr gameSettings = GameSettings::getInstance();
				if (gameSettings->isSpectatorAllowed()) {
					this->spectatorSystem->enable();
				}
				return true;
			}			

			const MouseState& mouseState = this->mouse->getMouseState();
			int xMouseRelative = (this->controls->invertMouseX) ? -mouseState.X.rel : mouseState.X.rel;
			int yMouseRelative = (this->controls->invertMouseY) ? -mouseState.Y.rel : mouseState.Y.rel;
			Vector3d cameraPosition(this->controls->mouseSensibility*static_cast<float>(xMouseRelative), this->controls->mouseSensibility*static_cast<float>(yMouseRelative), 0.0f);
			foreach (JoystickPtrMap::value_type entry, this->joysticks) {
				JoystickPtr joystick = entry.second;
				cameraPosition += joystick->getRightAxisPosition();
			}
			this->cameraController->update(static_cast<int>(cameraPosition.getX()), static_cast<int>(cameraPosition.getY()));
			this->human->updateManipulation(diffTime, static_cast<int>(cameraPosition.getX()));

			Quaternion actorOrientation = this->human->getActor()->getOrientation();
			Vector3d eyeDirection = rotate(inverse(actorOrientation), toAlgebra(this->camera->getDirection()));
			eyeDirection.setX(0.0f);
			eyeDirection.normalize();
			this->human->setEyeDirection(eyeDirection);
			
			if (this->human->isCrouched() && !this->isButtonDown(this->controls->crouch)) {
				this->human->stand();
			}
			if (!this->human->isSprinting() && !this->human->isCrouched() && this->isButtonDown(this->controls->crouch)) {
				this->human->crouch();
			}			

			bool move = false;
			float front = 0.0f;
			if (this->isButtonDown(this->controls->moveForward)) {
				move = true;
				front = 1.0f;
			}else if(this->isButtonDown(this->controls->moveBack)) {
				move = true;
				front = -0.5f;
			}
			float left = 0.0f;
			if (this->isButtonDown(this->controls->moveLeft)) {
				move = true;
				left = 1.0f;
			}else if(this->isButtonDown(this->controls->moveRight)) {
				move = true;
				left = -1.0f;
			}

			bool isRunning = !this->walkKeyHold;

			foreach(JoystickPtrMap::value_type entry, this->joysticks) {
				if (fabs(front) > 1.0e-3f || fabs(left) > 1.0e-3f) {
					break;
				}
				JoystickPtr joystick = entry.second;
				Vector3d position = joystick->getLeftAxisPosition();
				isRunning = (fabs(position.getX()) > 8) || (fabs(position.getY()) > 8);
				if (fabs(position.getX()) > 1.0e-3f) {
					if (position.getX() > 0.0f) {
						move = true;
						left = -1.0f;
					}else {
						move = true;
						left = 1.0f;
					}
				}
				if (fabs(position.getY()) > 1.0e-3f) {
					if (position.getY() > 0.0f) {
						move = true;
						front = -1.0f;
					}else {
						move = true;
						front = 1.0f;
					}
				}
			}

			if (front < 0.5f) {
				this->sprintKeyHit = false;
				this->human->stopSprint();
			}

			if (!move) {				
				this->human->idle();
				this->human->setMovementState(IDLE);
				this->humanInfo->stateInfo->idle();
				return true;
			}

			if (this->human->isSprinting() || this->sprintKeyHit) {
				this->sprintKeyHit = false;
				if (this->human->canSprint()) {
					this->human->sprint(left, front);
					this->human->setMovementState(SPRINTING);
					this->humanInfo->stateInfo->sprint(left, front);
					return true;
				}
			}

			if (isRunning) {
				this->human->stopSprint();
				this->human->run(left, front);
				this->human->setMovementState(RUNNING);
				this->humanInfo->stateInfo->run(left, front);
				return true;
			}
			
			this->human->stopSprint();
			this->human->walk(left, front);
			this->human->setMovementState(WALKING);
			this->humanInfo->stateInfo->walk(left, front);
			return true;
		}

		virtual void handlePause() {			
			this->sprintKeyHit = false;
			this->human->releaseObject();
			this->human->stopSprint();
			this->human->idle();
			this->human->setMovementState(IDLE);
			this->humanInfo->stateInfo->idle();
		}

		virtual bool handleEvent(float diffTime) {			
			this->human->updateBones(diffTime);
			this->human->localUpdate();
			this->localPlayerHumanEffects->updateEffects(this->camera);
			this->statueTimeToUpdateHandler->update();
			this->human->updateFlashlightBattery(diffTime);
			this->human->updateFlashlightIntensityFactor(diffTime);
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (this->human->isDead()) {
				return;
			}

			if (button == this->controls->grab && !this->mouseLeftButtonLocked) {
				if (this->human->tryPickingItem()) {
					if (this->human->getItemId() != -1) {
						this->itemSystem->requestAddItem(this->human->getCharacterType(), this->human->getItemId());
						return;
					}
					this->itemSystem->requestAddItemsFromDead(this->human->getCharacterType(), this->human->getDeadId());
					return;
				}
				if (this->human->tryPickingBattery()) {
					this->itemSystem->requestAddItem(this->human->getCharacterType(), this->human->getBatteryId()); 
					return;
				}
				if (this->human->tryGrabbingObject()) {
					return;
				}
			}
			if (button == this->controls->switchFlashlight && !this->mouseRightButtonLocked) {
				this->human->switchFlashlight();
			}

			if (button == this->controls->run) {
				this->sprintKeyHit = true;
			}
			if (button == this->controls->slowWalk) {
				this->walkKeyHold = true;
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (this->human->isDead()) {
				return;
			}
			if (button == this->controls->slowWalk) {
				this->walkKeyHold = false;
			}
			if (button == this->controls->grab) {
				this->human->releaseObject();
			}
		}

		void destroyCameraPositioner() {
			this->cameraPositioner->destroy();
		}
	};

	typedef SharedPointer<LocalPlayerHumanController> LocalPlayerHumanControllerPtr;
}

#endif
#ifndef TERROR_LOCAL_PLAYER_BLIND_CONTROLLER_H
#define TERROR_LOCAL_PLAYER_BLIND_CONTROLLER_H

#include "MonsterControllers/BlindController/LocalBlindController/LocalBlindController.h"
#include "ScareTrapSystem/ScareTrapSystem.h"
#include "ManipulationSystem/ManipulationSystem.h"
#include "SpectatorSystem/SpectatorSystem.h"
#include "DeathSystem/DeathSystem.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	class LocalPlayerBlindController : public LocalBlindController, public InputHandler, public EventHandler {
	public:
		LocalPlayerBlindController(BlindPtr monster, ScareTrapSystemPtr scareTrapSystem, DeathSystemPtr deathSystem, 
			ManipulationSystemPtr manipulationSystem, SpectatorSystemPtr spectatorSystem, RenderCameraPtr camera, 
			PostProcessingEffectPtr blindView, ControlsPtr controls, bool forest, bool factory)
			: LocalBlindController(monster), EventHandler(60), scareTrapSystem(scareTrapSystem), deathSystem(deathSystem), 
			manipulationSystem(manipulationSystem), blindView(blindView), controls(controls), walkKeyHold(false) 
		{
			camera->setOrientation(Ogre::Quaternion::IDENTITY);
			ActorPtr actor = this->monster->getActor();

			Vector3d offset(0.0f, 0.0f, 0.1f);
			BonePtr headBone = actor->getBone("bn_camera");
			this->cameraPositioner = CameraPositionerPtr(new BoneCameraPositioner(camera, actor, offset, headBone));

			CameraRotatorPtr cameraRotator(new CameraRotator(camera, actor, 80.0f));
			this->cameraController = CameraControllerPtr(new CameraController(cameraRotator, 0.005f, 1.0f));
			this->cameraController->setMouse(this->mouse);
			this->cameraController->setJoysticks(this->joysticks);

			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			/*this->cameraConnection = 
				mouseCursor->connectSensibilityChangedEvent(Bind(&CameraController::setMouseSensibility, this->cameraController, _1));*/
			
			if (forest || factory) {
				this->scareTrapSystem->reset(0.5f, true, 180.0f);
			}else {
				this->scareTrapSystem->reset(0.5f, true, 200.0f);
			}
			spectatorSystem->disable();

			this->blindView->setEnabled(true);
		}

		virtual void destroy() {
			this->monster->destroy();
			this->cameraConnection.disconnect();
			this->blindView->setEnabled(false);
		}

		virtual ~LocalPlayerBlindController() {}

	protected:
		CameraPositionerPtr cameraPositioner;
		CameraControllerPtr cameraController;
		ScareTrapSystemPtr scareTrapSystem;
		DeathSystemPtr deathSystem;
		ManipulationSystemPtr manipulationSystem;
		Connection cameraConnection;
		PostProcessingEffectPtr blindView;

		ControlsPtr controls;
		bool walkKeyHold;

	private:
		virtual MonsterControllerType getType() {
			return LOCAL_MONSTER_CONTROLLER;
		}

		virtual bool handleInput(float diffTime) {
			//this->monster->update(diffTime);

			const MouseState& mouseState = this->mouse->getMouseState();
			int xMouseRelative = (this->controls->invertMouseX) ? -mouseState.X.rel : mouseState.X.rel;
			int yMouseRelative = (this->controls->invertMouseY) ? -mouseState.Y.rel : mouseState.Y.rel;
			Vector3d cameraPosition(this->controls->mouseSensibility*static_cast<float>(xMouseRelative), this->controls->mouseSensibility*static_cast<float>(yMouseRelative), 0.0f);
			foreach (JoystickPtrMap::value_type entry, this->joysticks) {
				JoystickPtr joystick = entry.second;
				cameraPosition += joystick->getRightAxisPosition();
			}
			this->cameraController->update(static_cast<int>(cameraPosition.getX()), static_cast<int>(cameraPosition.getY()));	
			this->monster->updateManipulation(diffTime, cameraPosition.getX());

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

			if (!move) {
				this->monster->idle();
				this->monsterActionRequest->stateInfo->idle();
				return true;
			}

			if (isRunning) {
				this->monster->run(left, front);
				this->monsterActionRequest->stateInfo->run(left, front);
				return true;
			}
			
			this->monster->walk(left, front);
			this->monsterActionRequest->stateInfo->walk(left, front);
			return true;
		}

		virtual void handlePause() {
			this->monster->idle();
			this->monsterActionRequest->stateInfo->idle();
		}

		virtual bool handleEvent(float diffTime) {
			this->monster->localUpdate(diffTime);
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (button == this->controls->slowWalk) {
				this->walkKeyHold = true;
			}
			if (button == this->controls->attack) {
				this->monster->tryAttacking();
				this->deathSystem->requestKillCharacter(CharacterType(this->monster->getAttackActorId()));
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (button == this->controls->slowWalk) {
				this->walkKeyHold = false;
			}
		}
	};

	typedef SharedPointer<LocalPlayerBlindController> LocalPlayerBlindControllerPtr;
}

#endif
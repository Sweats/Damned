#ifndef TERROR_LOCAL_PLAYER_BERSERKER_CONTROLLER_H
#define TERROR_LOCAL_PLAYER_BERSERKER_CONTROLLER_H 

#include "MonsterControllers/BerserkerController/LocalBerserkerController/LocalBerserkerController.h"
#include "ScareTrapSystem/ScareTrapSystem.h"
#include "ManipulationSystem/ManipulationSystem.h"
#include "SpectatorSystem/SpectatorSystem.h"
#include "DeathSystem/DeathSystem.h"
#include "TerrorSave/TerrorSave.h"
#include "BerserkerEnergyRegenerationSystem/BerserkerEnergyRegenerationSystem.h"
#include "MonsterControllers/BerserkerController/LocalBerserkerController/LocalPlayerBerserkerController/LocalPlayerBerserkerEffects/LocalPlayerBerserkerEffects.h"
#include "BerserkerTeleportSystem/BerserkerTeleportSystem.h"

namespace terror {
	class LocalPlayerBerserkerController : public LocalBerserkerController, public InputHandler, public EventHandler {
	public:
		LocalPlayerBerserkerController(BerserkerPtr monster, ScareTrapSystemPtr scareTrapSystem, DeathSystemPtr deathSystem, ManipulationSystemPtr manipulationSystem, SpectatorSystemPtr spectatorSystem, 
			RenderCameraPtr camera, LocalPlayerBerserkerEffectsPtr localPlayerBerserkerEffects, BerserkerEnergyRegenerationSystemPtr berserkerEnergyRegenerationSystem, 
			BerserkerTeleportSystemPtr berserkerTeleportSystem, ControlsPtr controls)
			: LocalBerserkerController(monster), EventHandler(60), scareTrapSystem(scareTrapSystem), deathSystem(deathSystem), manipulationSystem(manipulationSystem), 
			localPlayerBerserkerEffects(localPlayerBerserkerEffects), berserkerTeleportSystem(berserkerTeleportSystem), controls(controls), 
			walkKeyHold(false)
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

			this->minEnergyForBerserkerModeEvent = 
				berserkerEnergyRegenerationSystem->connectMinEnergyForBerserkerEvent(Bind(&LocalPlayerBerserkerEffects::minEnergyForBerserkerEffects, localPlayerBerserkerEffects));
			this->minEnergyForTeleportingEvent = 
				berserkerEnergyRegenerationSystem->connectMinEnergyForTeleportingEvent(Bind(&LocalPlayerBerserkerEffects::minEnergyForTeleportingEffects, localPlayerBerserkerEffects));
			this->berserker->connectBecomeBerserkerEvent(Bind(&LocalPlayerBerserkerEffects::becomeBerserkerEffects, localPlayerBerserkerEffects));
			this->berserker->connectBecomeGhostEvent(Bind(&LocalPlayerBerserkerEffects::returnToGhostModeEffects, localPlayerBerserkerEffects));

			this->scareTrapSystem->notifyMonsterTeleported();
			spectatorSystem->disable();
		}

		virtual void destroy() {
			this->minEnergyForBerserkerModeEvent.disconnect();
			this->minEnergyForTeleportingEvent.disconnect();
			this->monster->destroy();
			this->localPlayerBerserkerEffects->resetAllEffects();
			this->cameraConnection.disconnect();
		}

		virtual ~LocalPlayerBerserkerController() {}

	protected:
		CameraPositionerPtr cameraPositioner;
		CameraControllerPtr cameraController;
		ScareTrapSystemPtr scareTrapSystem;
		DeathSystemPtr deathSystem;
		ManipulationSystemPtr manipulationSystem;
		Connection cameraConnection;
		LocalPlayerBerserkerEffectsPtr localPlayerBerserkerEffects;
		BerserkerTeleportSystemPtr berserkerTeleportSystem;
		ControlsPtr controls;
		Connection minEnergyForBerserkerModeEvent;
		Connection minEnergyForTeleportingEvent;
		bool walkKeyHold;

	private:
		virtual MonsterControllerType getType() {
			return LOCAL_MONSTER_CONTROLLER;
		}

		virtual bool handleInput(float diffTime) {
			this->berserker->update(diffTime);
			this->localPlayerBerserkerEffects->update();

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

			foreach(JoystickPtrMap::value_type entry, this->joysticks) {
				if (fabs(front) > 1.0e-3f || fabs(left) > 1.0e-3f) {
					break;
				}
				JoystickPtr joystick = entry.second;
				Vector3d position = joystick->getLeftAxisPosition();
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
				if (this->berserker->isInBerserkerMode()) {
					this->monster->monsterIdle();
				}else {
					this->monster->idle();
				}
				this->monsterActionRequest->stateInfo->idle();
				return true;
			}

			if (this->berserker->isInBerserkerMode()) {
				this->monster->run(left, front);
				this->monsterActionRequest->stateInfo->run(left, front);
				return true;
			}

			this->monster->walk(left, front);
			this->monsterActionRequest->stateInfo->walk(left, front);
			return true;
		}

		virtual void handlePause() {
			if (this->berserker->isInBerserkerMode()) {
				this->monster->monsterIdle();
			}else {
				this->monster->idle();
			}
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
			if (button == this->controls->transform) {
				this->berserker->tryBerserker();
			}
			if (button == this->controls->grab) {
				if (this->monster->tryGrabbingObject()) {
					return;
				}
			}
			if (button == this->controls->attack && this->berserker->isInBerserkerMode()) {
				if (this->monster->tryAttacking()) {
					this->deathSystem->requestKillCharacter(CharacterType(this->monster->getAttackActorId()));
					return;
				}
			}
			if (button == this->controls->teleport) {
				if (!this->berserkerTeleportSystem->tryTeleport()) {
					writeMessage("Not enough energy to teleport");	
				}
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (button == this->controls->slowWalk) {
				this->walkKeyHold = false;
			}
			if (button == this->controls->grab) {
				this->monster->releaseObject();
			}
		}
	};

	typedef SharedPointer<LocalPlayerBerserkerController> LocalPlayerBerserkerControllerPtr;
}

#endif
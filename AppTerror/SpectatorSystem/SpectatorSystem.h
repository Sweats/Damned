#ifndef TERROR_SPECTATOR_SYSTEM_H
#define TERROR_SPECTATOR_SYSTEM_H

#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class SpectatorSystem : public InputHandler {
	public:
		SpectatorSystem(CharacterHandlerPtr characterHandler, RenderCameraPtr camera, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave)
			: characterHandler(characterHandler), camera(camera), terrorClient(terrorClient), terrorSave(terrorSave), enabled(false), timeToStart(-1.0f) {}

		Connection connectStartedEvent(const BasicSignal::slot_type& startedEvent) {
			return this->startedSignal.connect(startedEvent);
		}

		void enable() {
			if (this->enabled) {
				return;
			}
			this->enabled = true;
			this->timeToStart = 5.0f;
		}

		void disable() {			
			if (!this->enabled) {
				return;
			}
			this->humanSpectating.reset();			
			this->enabled = false;
			if (this->cameraPositioner) {
				this->cameraPositioner->destroy();
			}
		}

		void notifyStageLoaded(StagePtr stage) {			
			GameSettingsPtr gameSettings = GameSettings::getInstance();

			PlayersStatePtr playersState = PlayersState::getInstance();			
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());

			CharacterType character = CHAR_RANDOM;
			if (playerInfo) {
				character = playerInfo->getCharacter();
			}

			if (gameSettings->isSpectatorAllowed() && character == CHAR_RANDOM) {
				this->enabled = true;
				this->timeToStart = 1.0f;
			}
		}

		void notifyStageUnloaded() {
			this->disable();
		}

		virtual ~SpectatorSystem() {}

	protected:
		CharacterHandlerPtr characterHandler;		
		CameraPositionerPtr cameraPositioner;
		HumanPtr humanSpectating;
		RenderCameraPtr camera;
		TerrorClientPtr terrorClient;
		BasicSignal startedSignal;
		TerrorSavePtr terrorSave;
		float timeToStart;
		bool enabled;

	private:
		virtual bool handleInput(float diffTime) {
			if (!this->enabled || this->timeToStart < 0.0f || GameSettings::getInstance()->getGameModeType() == LIFE_MODE) {
				return true;
			}
			this->timeToStart -= diffTime;
			if (this->timeToStart < 0.0f) {
				this->timeToStart = -1.0f;
				this->startedSignal();
				this->spectateNextLivingHuman();
			}
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (!this->enabled) { 
				return;
			}
			if (button == this->terrorSave->controls->spectateNext) {
				this->timeToStart = -1.0f;
				this->startedSignal();
				this->spectateNextLivingHuman();
			}
		}

		void spectateNextLivingHuman() {
			this->humanSpectating = this->getNextLivingHuman();
			if (!this->humanSpectating) {
				return;
			}
			
			try {
				PlayersStatePtr playersState = PlayersState::getInstance();
				PlayerInfoPtr playerInfo = playersState->getPlayerInfoByCharacter(this->humanSpectating->getCharacterType());
				
				ActorPtr actor = this->humanSpectating->getActor();
				Vector3d offset(0.0f, 0.0f, 0.1f);
				BonePtr headBone = actor->getBone("bn_camera");
				this->cameraPositioner = CameraPositionerPtr(new BoneCameraPositioner(this->camera, actor, offset, headBone, true));

				std::string message = std::string("Spectating player ") + playerInfo->getName() + std::string("!");
				writeMessage(message);
			}catch (std::exception& e) {
				this->humanSpectating.reset();
				return;
			}
		}

		HumanPtr getNextLivingHuman() {
			bool next = false;
			HumanPtrMap& humans = this->characterHandler->getHumansAlive();
			foreach (HumanPtrMap::value_type& entry, humans) {
				HumanPtr human = entry.second;
				if (!human->isDead()) {
					if (!this->humanSpectating) {
						return human;
					}
					if (next) {
						return human;
					}else if (human == this->humanSpectating) {
						next = true;
					}
				}				
			}
			foreach (HumanPtrMap::value_type& entry, humans) {
				HumanPtr human = entry.second;
				if (!human->isDead()) {
					if (next) {
						return human;
					}
				}	
			}
			return HumanPtr();
		}
	};

	typedef SharedPointer<SpectatorSystem> SpectatorSystemPtr;
}

#endif
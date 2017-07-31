#ifndef TERROR_LIFE_MODE_H
#define TERROR_LIFE_MODE_H

#include "GameMode/GameMode.h"
#include "GameStateHandler/GameStateHandler.h"

namespace terror {
	typedef Signal<void (HumanPtr)> HumanSignal;

	class LifeMode : public GameMode, public EventHandler {
	public:
		LifeMode(EndGameHandlerPtr endGameHandler, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient, int numberOfLives = 1) 
			: GameMode(endGameHandler), characterHandler(characterHandler), terrorClient(terrorClient), numberOfLives(numberOfLives), timeToRespawnLocalHuman(-1.0f), ingame(false) 
		{
			PlayersState::getInstance()->setNumberOfLives(this->numberOfLives);
		}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getGameModeType() == LIFE_MODE) {
				this->setNumberOfLives(gameSettings->getNumberOfLives());
			}
		}

		void notifyStageUnloaded() {
			this->ingame = false;
		}

		void setNumberOfLives(int numberOfLives) {
			if (numberOfLives < 0) {
				throw std::runtime_error("Trying to set a negative number of lives.");
			}
			this->numberOfLives = numberOfLives;
			PlayersState::getInstance()->setNumberOfLives(this->numberOfLives);
		}

		int getNumberOfLives() {
			return this->numberOfLives;
		}

		virtual void handleHumanDeath(HumanPtr human, bool playerDisconnected) {
			if (playerDisconnected) {
				return;
			}
			if (this->useLife()) {
				this->requestCharacterRespawn(human);
			}else if (this->terrorClient->isHost()) {
				this->endGameHandler->requestEndGame(MONSTER_WINS);
			}		
		}

		Connection connectLocalHumanRespawningEvent(const HumanSignal::slot_type& localHumanRespawningEvent) {
			return this->localHumanRespawningSignal.connect(localHumanRespawningEvent);
		}

		virtual ~LifeMode() {}

	protected:
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		HumanSignal localHumanRespawningSignal; 
		HumanPtr localHuman;
		int numberOfLives;
		float timeToRespawnLocalHuman;
		bool ingame;

	private:
		virtual bool handleEvent(float diffTime) {
			if (this->timeToRespawnLocalHuman < 0.0f) {
				return true;
			}
			this->timeToRespawnLocalHuman -= diffTime;
			if (this->timeToRespawnLocalHuman < 0.0f) {
				this->timeToRespawnLocalHuman = -1.0f;
				this->localHumanRespawningSignal(this->localHuman);
			}
			return true;
		}

		void requestCharacterRespawn(HumanPtr human) {
			std::string message = lexical_cast<string>(this->numberOfLives) + std::string(" lives left!");
			writeMessage(message);

			if (this->isHumanLocal(human)) {
				this->localHuman = human;
				this->timeToRespawnLocalHuman = 5;
			}
		}

		bool isHumanLocal(HumanPtr human) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return false;
			}

			CharacterType localCharacter = playerInfo->getCharacter();
			return human->getCharacterType() == localCharacter;
		}

		bool useLife() {
			--this->numberOfLives;
			PlayersState::getInstance()->setNumberOfLives(this->numberOfLives);
			if (this->numberOfLives <= 0) {
				return false;
			}			
			return true;
		}
	};

	typedef SharedPointer<LifeMode> LifeModePtr;
}

#endif
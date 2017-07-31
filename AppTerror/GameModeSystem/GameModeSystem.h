#ifndef TERROR_GAME_MODE_SYSTEM_H
#define TERROR_GAME_MODE_SYSTEM_H

#include "GameMode/NormalMode/NormalMode.h"
#include "GameMode/LifeMode/LifeMode.h"

namespace terror {
	typedef unordered_map<int, GameModePtr> GameModePtrMap;

	class GameModeSystem {
	public:
		GameModeSystem(EndGameHandlerPtr endGameHandler, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient) : terrorClient(terrorClient) {
			this->buildGameModes(endGameHandler, characterHandler);
		}

		void handleHumanDeath(HumanPtr human, bool playerDisconnected) {
			this->gameModes[GameSettings::getInstance()->getGameModeType()]->handleHumanDeath(human, playerDisconnected);
		}

		GameModePtrMap& getGameModes() {
			return this->gameModes;
		}

		virtual ~GameModeSystem() {}

	protected:
		GameModePtrMap gameModes;
		TerrorClientPtr terrorClient;

	private:
		void buildGameModes(EndGameHandlerPtr endGameHandler, CharacterHandlerPtr characterHandler) {
			this->gameModes[NORMAL_MODE] = GameModePtr(new NormalMode(endGameHandler, characterHandler, this->terrorClient));
			this->gameModes[LIFE_MODE] = GameModePtr(new LifeMode(endGameHandler, characterHandler, this->terrorClient));
		}
	};

	typedef SharedPointer<GameModeSystem> GameModeSystemPtr; 
}

#endif
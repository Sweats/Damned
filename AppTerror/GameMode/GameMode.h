#ifndef TERROR_GAME_MODE_H
#define TERROR_GAME_MODE_H

#include "EndGameHandler/EndGameHandler.h"
#include "GameStateHandler/GameStateHandler.h"

namespace terror {
	class GameMode {
	public:
		GameMode(EndGameHandlerPtr endGameHandler)
			: endGameHandler(endGameHandler) {}

		virtual void handleHumanDeath(HumanPtr human, bool playerDisconnected) {}

		virtual ~GameMode() {}
		
	protected:
		EndGameHandlerPtr endGameHandler;
	};

	typedef SharedPointer<GameMode> GameModePtr; 
}

#endif
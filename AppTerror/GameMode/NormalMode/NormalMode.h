#ifndef TERROR_NORMAL_MODE_H
#define TERROR_NORMAL_MODE_H

#include "GameMode/GameMode.h"

namespace terror {
	class NormalMode : public GameMode {
	public:
		NormalMode(EndGameHandlerPtr endGameHandler, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient) 
			: GameMode(endGameHandler), characterHandler(characterHandler), terrorClient(terrorClient) {}

		virtual void handleHumanDeath(HumanPtr human, bool playerDisconnected) {
			if (this->terrorClient->isHost() && this->characterHandler->getHumansAlive().empty()) {
				this->endGameHandler->requestEndGame(MONSTER_WINS);
			}
		}

		virtual ~NormalMode() {}

	protected:
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
	};

	typedef SharedPointer<NormalMode> NormalModePtr;
}

#endif
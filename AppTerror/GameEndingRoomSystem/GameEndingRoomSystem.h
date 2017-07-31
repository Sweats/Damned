#ifndef TERROR_GAME_ENDING_ROOM_SYSTEM_H
#define TERROR_GAME_ENDING_ROOM_SYSTEM_H

#include "GameEndingRoom/GameEndingRoom.h"
#include "EndGameHandler/EndGameHandler.h"

namespace terror {
	typedef PointerList<GameEndingRoom> GameEndingRoomPtrList;

	class GameEndingRoomSystem : public EventHandler {
	public:
		GameEndingRoomSystem(EndGameHandlerPtr endGameHandler, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient) 
			: EventHandler(30), endGameHandler(endGameHandler), characterHandler(characterHandler), terrorClient(terrorClient), timeToCheck(0.05f), time(0.0f) {}

		void notifyStageLoaded(StagePtr stage) {
			ObjectPtrListPtr objects = stage->getObjectsWithMask(501);
			foreach (ObjectPtr object, *objects) {
				GameEndingRoomPtr gameEndingRoom(new GameEndingRoom(object));
				this->gameEndingRooms.add(gameEndingRoom);
			}
		}

		void notifyStageUnloaded() {
			this->gameEndingRooms.clear();
		}

		virtual ~GameEndingRoomSystem() {}

	protected:
		EndGameHandlerPtr endGameHandler;
		GameEndingRoomPtrList gameEndingRooms;
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		float timeToCheck;
		float time;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->terrorClient->isHost()) {
				return true;
			}

			this->time += diffTime;
			if (this->time < this->timeToCheck) {
				return true;
			}
			this->time = 0.0f;

			foreach (GameEndingRoomPtr gameEndingRoom, this->gameEndingRooms) {
				if (gameEndingRoom->isAllHumansInside(this->characterHandler)) {
					this->endGameHandler->requestEndGame(SURVIVORS_WINS);
					this->gameEndingRooms.clear();
					return true;
				}
			}
			return true;
		}
	};

	typedef SharedPointer<GameEndingRoomSystem> GameEndingRoomSystemPtr; 
}

#endif
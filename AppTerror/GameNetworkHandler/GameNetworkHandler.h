#ifndef TERROR_GAME_NETWORK_HANDLER_H
#define TERROR_GAME_NETWORK_HANDLER_H

#include "GameStateHandler/GameStateHandler.h"
#include "CharacterLoader/CharacterLoader.h"
#include "StageHandler/StageHandler.h"

namespace terror {
	typedef unordered_map<int, float> IntFloatMap;

	class GameNetworkHandler : public NetworkObserver, public EventHandler {
	public:
		GameNetworkHandler(TerrorClientPtr terrorClient, GameStateHandlerPtr gameStateHandler, CharacterLoaderPtr characterLoader, ManipulationSystemPtr manipulationSystem) 
			: EventHandler(30), NetworkObserver(false), terrorClient(terrorClient), gameStateHandler(gameStateHandler), characterLoader(characterLoader), timeToIgnoreMonsterActionRequest(-1.0f),
			manipulationSystem(manipulationSystem), playerJoinedGameCanBeSent(false) 
		{
			this->disconnectSound = Core::getInstance()->createSound("player_disconnect.ogg");
		}

		void notifyStageLoaded(StagePtr stage) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			CharacterType character = playerInfo->getCharacter();
			int spawnPointId = playerInfo->getSpawnPointId();			
			if (isMonsterCharacter(character)) {
				SpawnPointPtr spawnPoint = stage->getScene()->getSpawnPoint(spawnPointId);
				MonsterControllerPtr controller = this->characterLoader->loadLocalPlayerMonster(character, spawnPoint, stage->getName() == "Black_Lake_Woods", stage->getName() == "Factory", stage->getName());
				this->gameStateHandler->setMonsterController(controller);
			}else {
				int monsterSpawnPoint = 4;
				CharacterType monsterChar = playersState->getMonsterCharacter();
				if (!isMonsterCharacter(monsterChar)) {
					return;
				}
				if (this->terrorClient->isHost() && !playersState->monsterPlayerExists()) {
					SpawnPointPtr spawnPoint = stage->getScene()->getSpawnPoint(monsterSpawnPoint);
					MonsterControllerPtr controller = this->characterLoader->loadLocalAiMonster(monsterChar, spawnPoint, stage->getName() == "Black_Lake_Woods", stage->getName() == "Factory", stage->getName());
					this->gameStateHandler->setMonsterController(controller);
				}else {
					SpawnPointPtr spawnPoint = stage->getScene()->getSpawnPoint(monsterSpawnPoint);
					MonsterControllerPtr controller = this->characterLoader->loadRemotePlayerMonster(monsterChar, spawnPoint, stage->getName() == "Black_Lake_Woods", stage->getName() == "Factory", stage->getName());
					this->gameStateHandler->setMonsterController(controller);
				}
				if (character != CHAR_NONE && character != CHAR_RANDOM && spawnPointId != -1) {
					SpawnPointPtr spawnPoint = stage->getScene()->getSpawnPoint(spawnPointId);
					LocalPlayerHumanControllerPtr controller = this->characterLoader->loadLocalPlayerHuman(character, spawnPoint, (stage->getName() == "Black_Lake_Woods"));
					this->gameStateHandler->addHumanController(controller, true);
				}
			}
			this->timeToIgnoreMonsterActionRequest = -1.0f;
		}

		void notifyStageUnloaded() {	
			this->manipulationSystem->reset();
			this->characterLoader->destroyMonsterController();
			foreach (HumanControllerPtrMap::value_type& entry, this->gameStateHandler->getHumanControllers()) {
				this->characterLoader->destroyHumanController(entry.second);
			}

			foreach (HumanControllerPtr humanController, this->gameStateHandler->getDeadHumanControllers()) {
				this->characterLoader->destroyHumanController(humanController);
			}

			LocalPlayerHumanControllerPtr localHumanController = this->gameStateHandler->getLocalHumanController();
			if (localHumanController && localHumanController->getHuman()->isDead()) {
				this->characterLoader->destroyHumanController(localHumanController);
			}

			this->gameStateHandler->reset();
			this->timeToIgnoreCharacters.clear();
			this->timeToIgnoreMonsterActionRequest = -1.0f;
			this->playerJoinedGameCanBeSent = false;
		}

		void notifyHumanDeath(HumanPtr human) {
			if (!this->terrorClient->isHost()) {
				return;
			}
			this->timeToIgnoreCharacters[human->getCharacterType()] = 5.0f;
		}

		void notifyMonsterTeleported() {
			this->timeToIgnoreMonsterActionRequest = 0.1f;
		}

		virtual ~GameNetworkHandler() {}

	protected:
		TerrorClientPtr terrorClient;
		GameStateHandlerPtr gameStateHandler;
		CharacterLoaderPtr characterLoader;
		ManipulationSystemPtr manipulationSystem;
		bool playerJoinedGameCanBeSent;

		IntFloatMap timeToIgnoreCharacters;
		float timeToIgnoreMonsterActionRequest;

		SoundPtr disconnectSound;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->gameStateHandler->getStage()) {
				return true;
			}
			this->setFrequency(this->terrorClient->isHost() ? 20 : 30);

			if (this->timeToIgnoreMonsterActionRequest >= 0.0f) {
				this->timeToIgnoreMonsterActionRequest -= diffTime;
			}

			LocalPlayerHumanControllerPtr humanController = this->gameStateHandler->getLocalHumanController();
			if (humanController && !humanController->getHuman()->isDead()) {
				HumanInfoPtr humanInfo = humanController->getHumanInfo();
				if (!this->terrorClient->isHost()) {
					this->terrorClient->sendObjectToHost(humanInfo);
					return true;
				}
				this->gameStateHandler->applyHumanInfo(humanInfo);
			}

			MonsterControllerPtr monsterController = this->gameStateHandler->getMonsterController();
			if (monsterController && monsterController->getType() != REMOTE_MONSTER_CONTROLLER) {
				MonsterActionRequestPtr monsterActionRequest;
				MonsterType monsterType = monsterController->getMonster()->getType();
				if (monsterType == SPECTRUM) {
					monsterActionRequest = static_ptr_cast<LocalSpectrumController>(monsterController)->getMonsterActionRequest();
				}else if (monsterType == BLIND) {
					monsterActionRequest = static_ptr_cast<LocalBlindController>(monsterController)->getMonsterActionRequest();
				}else if (monsterType == BERSERKER) {
					monsterActionRequest = static_ptr_cast<LocalBerserkerController>(monsterController)->getMonsterActionRequest();
				}else if (monsterType == STATUE) {
					monsterActionRequest = static_ptr_cast<LocalStatueController>(monsterController)->getMonsterActionRequest();
				}

				if (!this->terrorClient->isHost()) {
					this->terrorClient->sendObjectToHost(monsterActionRequest);
					return true;
				}
				this->gameStateHandler->applyMonsterActionRequest(monsterActionRequest);
			}			

			if (!this->terrorClient->isHost()) {
				return true;
			}

			foreach (IntFloatMap::value_type& entry, this->timeToIgnoreCharacters) {
				float& timeToIgnoreCharacter = entry.second;
				timeToIgnoreCharacter -= diffTime;
				if (timeToIgnoreCharacter < 0.0f) {
					this->timeToIgnoreCharacters.erase(entry.first);
					break;
				}
			}

			GameStatePtr gameState = this->gameStateHandler->getGameState();
			if (this->timeToIgnoreMonsterActionRequest > 0.0f) {
				gameState->charactersGameState->ignoreMonsterInfo = true;
			}else {
				gameState->charactersGameState->ignoreMonsterInfo = false;
			}
			this->terrorClient->sendObjectToPlayers(gameState->charactersGameState);
			if (gameState->objectsGameState->objects.size()) {
				this->terrorClient->sendObjectToPlayers(gameState->objectsGameState);
			}
			if (gameState->destroyedObjectsGameState->destroyedObjectIds.size()) {
				this->terrorClient->sendObjectToPlayers(gameState->destroyedObjectsGameState, true);
			}
			return true;
		}

		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			int packetLabel = tcpPacket->getLabel();

			if (packetLabel == REMOVE_PLAYER) {
				this->disconnectSound->play();

				if (!this->gameStateHandler->getStage()) {
					return;
				}

				RemovePlayerPtr removePlayer(new RemovePlayer);
				tcpPacket->buildObject(removePlayer);

				PlayersStatePtr playersState = PlayersState::getInstance();
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(removePlayer->getId());
				if (!playerInfo) {
					return;
				}

				std::string message = std::string("Player ") + playerInfo->getName() + std::string(" disconnected!");
				writeMessage(message);

				if (this->gameStateHandler->notifyHumanDisconnected(playerInfo->getCharacter())) {
					return;
				}
				if (this->terrorClient->isHost() && isMonsterCharacter(playerInfo->getCharacter())) {
					MonsterControllerPtr monsterController = this->gameStateHandler->getMonsterController();
					if (monsterController && monsterController->getType() == REMOTE_MONSTER_CONTROLLER) {
						monsterController = this->characterLoader->transformMonsterControllerInAi();
						this->gameStateHandler->setMonsterController(monsterController);
					}
				}
				return;
			}

			if (packetLabel == HOST_INFO) {
				PlayersStatePtr playersState = PlayersState::getInstance();
				if (!playersState->monsterPlayerExists()) {
					MonsterControllerPtr monsterController = this->gameStateHandler->getMonsterController();
					if (monsterController && monsterController->getType() == REMOTE_MONSTER_CONTROLLER) {
						monsterController = this->characterLoader->transformMonsterControllerInAi();
						this->gameStateHandler->setMonsterController(monsterController);
					}
				}
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			int packetLabel = udpPacket->getLabel();

			if (packetLabel == CHARACTERS_GAME_STATE) {
				if (!this->gameStateHandler->getStage()) {
					return;
				}

				CharactersGameStatePtr charactersGameState(new CharactersGameState);
				if (!udpPacket->buildObject(charactersGameState)) {
					return;
				}

				foreach (HumanInfoPtr humanInfo, charactersGameState->humans) {
					if (!this->gameStateHandler->isHumanAlive(humanInfo->character)) {
						RemotePlayerHumanControllerPtr controller = this->characterLoader->loadRemotePlayerHuman(humanInfo->character, (this->gameStateHandler->getStage()->getName() == "Black_Lake_Woods"));
						this->gameStateHandler->addHumanController(controller);
					}
					this->gameStateHandler->updateHumanInfo(humanInfo);
				}

				if (this->gameStateHandler->monsterExists() && !charactersGameState->ignoreMonsterInfo) {
					this->gameStateHandler->updateMonsterInfo(charactersGameState->monster);
				}

				if (this->playerJoinedGameCanBeSent) {
					PlayerJoinedGamePtr playerJoinedGame(new PlayerJoinedGame(this->terrorClient->getId()));
					this->terrorClient->sendObjectToHost(playerJoinedGame, true);
					this->playerJoinedGameCanBeSent = false;
				}
				return;
			}

			if (packetLabel == OBJECTS_GAME_STATE) {
				if (!this->gameStateHandler->getStage()) {
					return;
				}

				ObjectsGameStatePtr objectsGameState(new ObjectsGameState);
				if (!udpPacket->buildObject(objectsGameState)) {
					return;
				}

				foreach (ObjectInfoPtr objectInfo, objectsGameState->objects) {
					this->gameStateHandler->updateObjectInfo(objectInfo);
				}
				return;
			}

			if (packetLabel == DESTROYED_OBJECTS_GAME_STATE) {
				if (!this->gameStateHandler->getStage()) {
					return;
				}

				DestroyedObjectsGameStatePtr destroyedObjectsGameState(new DestroyedObjectsGameState);
				if (!udpPacket->buildObject(destroyedObjectsGameState)) {
					return;
				}

				this->gameStateHandler->setDestroyedObjects(destroyedObjectsGameState->destroyedObjectIds);
				return;
			}

			if (packetLabel == HUMAN_INFO) {
				if (!this->gameStateHandler->getStage()) {
					return;
				}

				HumanInfoPtr humanInfo(new HumanInfo);
				if (!udpPacket->buildObject(humanInfo)) {
					return;
				}

				float timeToIgnoreCharacter = this->getTimeToIgnoreCharacter(humanInfo->character);
				if (timeToIgnoreCharacter > 0.0f) {
					return;
				}

				if (!this->gameStateHandler->isHumanAlive(humanInfo->character)) {
					RemotePlayerHumanControllerPtr controller = this->characterLoader->loadRemotePlayerHuman(humanInfo->character, (this->gameStateHandler->getStage()->getName() == "Black_Lake_Woods"));
					HumanPtr human = controller->getHuman();
					human->getActor()->setPosition(humanInfo->stateInfo->getPosition());
					this->gameStateHandler->addHumanController(controller);
					return;
				}
				this->gameStateHandler->applyHumanInfo(humanInfo);
				return;
			}

			if (packetLabel == MONSTER_ACTION_REQUEST) {
				MonsterActionRequestPtr monsterActionRequest(new MonsterActionRequest);
				if (!udpPacket->buildObject(monsterActionRequest)) {
					return;
				}

				if (this->timeToIgnoreMonsterActionRequest > 0.0f) {
					return;
				}

				MonsterControllerPtr monsterController = this->gameStateHandler->getMonsterController();
				if (monsterController && monsterController->getType() == AI_MONSTER_CONTROLLER) {
					monsterController = this->characterLoader->transformMonsterControllerInRemote();
					this->gameStateHandler->setMonsterController(monsterController);
				}

				if (!this->gameStateHandler->monsterExists()) {
					return;
				}
				this->gameStateHandler->applyMonsterActionRequest(monsterActionRequest);
				return;
			}

			if (packetLabel == STAGE_STATE) {
				this->playerJoinedGameCanBeSent = true;
			}

		}

		float getTimeToIgnoreCharacter(CharacterType character) {
			IntFloatMap::const_iterator entry = this->timeToIgnoreCharacters.find(character);
			if (entry == this->timeToIgnoreCharacters.end()) {
				return -1.0f;
			}
			return entry->second;
		}
	};

	typedef SharedPointer<GameNetworkHandler> GameNetworkHandlerPtr;
}

#endif
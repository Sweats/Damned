#ifndef TERROR_CHARACTER_SELECTION_WINDOW_HANDLER_H
#define TERROR_CHARACTER_SELECTION_WINDOW_HANDLER_H

#include "Packets/Packets.h"
#include "PlayerProfile/PlayerProfile.h"
#include "GameStateHandler/GameStateHandler.h"
#include "CharacterLoader/CharacterLoader.h"
#include "Packets/PlayersStateAck/PlayersStateAck.h"

namespace terror {	
	class CharacterSelectionWindowHandler : public NetworkObserver {
	public:
		CharacterSelectionWindowHandler(GuiLayoutPtr lobbyLayout, GuiLayoutPtr characterSelectionLayout, TerrorClientPtr terrorClient,
			GameStateHandlerPtr gameStateHandler, CharacterLoaderPtr characterLoader, PlayerProfilePtr playerProfile, SoundPtr playerConnectedSound)
			: NetworkObserver(false), lobbyLayout(lobbyLayout), characterSelectionLayout(characterSelectionLayout), terrorClient(terrorClient), 
			gameStateHandler(gameStateHandler), characterLoader(characterLoader), playerProfile(playerProfile), playerConnectedSound(playerConnectedSound),
			ingame(false)
		{
			this->char0Button = characterSelectionLayout->getWidget("Char0Button");
			this->char0Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->char1Button = characterSelectionLayout->getWidget("Char1Button");
			this->char1Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->char2Button = characterSelectionLayout->getWidget("Char2Button");
			this->char2Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->char3Button = characterSelectionLayout->getWidget("Char3Button");
			this->char3Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->monster0Button = characterSelectionLayout->getWidget("Monster0Button");
			this->monster0Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->monster1Button = characterSelectionLayout->getWidget("Monster1Button");
			this->monster1Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->monster2Button = characterSelectionLayout->getWidget("Monster2Button");
			this->monster2Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->monster3Button = characterSelectionLayout->getWidget("Monster3Button");
			this->monster3Button->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);
			this->randomButton = characterSelectionLayout->getWidget("RandomButton");
			this->randomButton->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleButton);

			this->cancelButton = characterSelectionLayout->getWidget("CancelButton");
			this->cancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &CharacterSelectionWindowHandler::handleCancelButton);
		}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
		}

		void notifyGameSettingsUpdated() {
			if (this->terrorClient->getId() == -1) {
				return;
			}
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			CharacterType character = playerInfo->getCharacter();

			this->updateButtonsAvailability();		
			
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (this->ingame) {
				if (character != CHAR_RANDOM) {
					return;
				}
				if (gameSettings->getCharacterSelectionMode() == ALL_RANDOM_CHARACTERS) {
					int id = this->terrorClient->getId();
					CharSelectionRequestPtr charSelectionRequest(new CharSelectionRequest(CHAR_RANDOM, id));
					this->terrorClient->sendObjectToHost(charSelectionRequest, true);
				}else {
					if (this->char0Button->getEnabled() || this->char1Button->getEnabled() || this->char2Button->getEnabled() ||
						this->char3Button->getEnabled() || 	this->monster0Button->getEnabled() || this->monster1Button->getEnabled() || 
						this->monster2Button->getEnabled() || this->monster3Button->getEnabled()) 
					{	
						MouseCursorPtr mouseCursor = MouseCursor::getInstance();
						mouseCursor->show();
						this->characterSelectionLayout->show();
						this->cancelButton->setEnabled(false);

					}else {
						writeMessage("No living character left, you'll have to wait for the next game.");
					}
				}
			}else {				
				if (gameSettings->getCharacterSelectionMode() == ALL_RANDOM_CHARACTERS) {
					for (int i = 0; i < 5; ++i) {
						PlayerInfoPtr playerInfo = playersState->getPlayerInfo(i);
						if (playerInfo && playerInfo->getName() != "") {
							playerInfo->setCharacter(CHAR_RANDOM);
							playerInfo->updateGuiElements();
						}
					}
				}else if (playersState->monsterPlayerExists()) {
					PlayerInfoPtr playerInfo = playersState->getMonsterPlayerInfo();
					if (playerInfo->getCharacter() == CHAR_MONSTER_0 && !gameSettings->getMonsters()[SPECTRUM] || 
						playerInfo->getCharacter() == CHAR_MONSTER_1 && !gameSettings->getMonsters()[BLIND] ||
						playerInfo->getCharacter() == CHAR_MONSTER_2 && !gameSettings->getMonsters()[BERSERKER] ||
						playerInfo->getCharacter() == CHAR_MONSTER_3 && !gameSettings->getMonsters()[STATUE]) 
					{
						playerInfo->setCharacter(CHAR_RANDOM);
						playerInfo->updateGuiElements();
					}
				}
			}
		}	

		void notifyStageUnloaded() {
			this->char0Button->setEnabled(true);
			this->char1Button->setEnabled(true);
			this->char2Button->setEnabled(true);
			this->char3Button->setEnabled(true);
			this->monster0Button->setEnabled(true);
			this->monster1Button->setEnabled(true);
			this->monster2Button->setEnabled(true);
			this->monster3Button->setEnabled(true);
			this->cancelButton->setEnabled(true);	

			this->ingame = false;

			if (!this->terrorClient->isHost()) {
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			for (int i = 0; i < 5; ++i) {
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(i);
				if (playerInfo && playerInfo->getName() != "") {
					playerInfo->setCharacter(CHAR_RANDOM);
					playerInfo->updateGuiElements();
				}
			}
			playersState->resetDeadCharacters();
	
			this->terrorClient->sendObjectToPlayers(playersState);
		}

		void reset() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			playersState->reset();
			this->char0Button->setEnabled(true);
			this->char1Button->setEnabled(true);
			this->char2Button->setEnabled(true);
			this->char3Button->setEnabled(true);
			this->monster0Button->setEnabled(true);
			this->monster1Button->setEnabled(true);
			this->monster2Button->setEnabled(true);
			this->monster3Button->setEnabled(true);
			this->cancelButton->setEnabled(true);
		}

		virtual ~CharacterSelectionWindowHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr characterSelectionLayout;
		TerrorClientPtr terrorClient;
		GameStateHandlerPtr gameStateHandler;
		CharacterLoaderPtr characterLoader;
		PlayerProfilePtr playerProfile;
		SoundPtr playerConnectedSound;
		Widget* char0Button;
		Widget* char1Button;
		Widget* char2Button;
		Widget* char3Button;
		Widget* monster0Button;
		Widget* monster1Button;
		Widget* monster2Button;
		Widget* monster3Button;
		Widget* randomButton;
		Widget* cancelButton;
		bool ingame;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == HOST_INFO) {
				int id = this->terrorClient->getId();
				string playerName = this->playerProfile->getUsername() + string(" (Host)");

				PlayersStatePtr playersState = PlayersState::getInstance();
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(id);
				if (!playerInfo) {
					return;
				}

				playerInfo->setName(playerName);
				if (playerInfo->getCharacter() == CHAR_NONE) {
					playerInfo->setCharacter(CHAR_RANDOM);
				}
				playerInfo->updateGuiElements();
				return;
			}			
			if (tcpPacket->getLabel() == REMOVE_PLAYER) {
				RemovePlayerPtr removePlayer(new RemovePlayer);
				tcpPacket->buildObject(removePlayer);

				PlayersStatePtr playersState = PlayersState::getInstance();
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(removePlayer->getId());
				if (!playerInfo) {
					return;
				}

				CharacterType character = playerInfo->getCharacter();
				if (!this->gameStateHandler->isHumanDead(character)) {
					Widget* button = this->getCharacterButton(character);
					if (button) {
						button->setEnabled(true);
					}

					if (!this->ingame && isMonsterCharacter(character)) {
						GameSettingsPtr gameSettings = GameSettings::getInstance();
						if (gameSettings->getMonsters()[SPECTRUM]) {
							this->monster0Button->setEnabled(true);
						}
						if (gameSettings->getMonsters()[BLIND]) {
							this->monster1Button->setEnabled(true);
						}
						if (gameSettings->getMonsters()[BERSERKER]) {
							this->monster2Button->setEnabled(true);
						}
						if (gameSettings->getMonsters()[STATUE]) {
							this->monster3Button->setEnabled(true);
						}
					}
				}

				playerInfo->setName("");
				playerInfo->setCharacter(CHAR_NONE);
				playerInfo->setSpawnPointId(0);
				playerInfo->resetUdpAddress();
				playerInfo->updateGuiElements();
				return;
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == JOIN_ACCEPTED) {
				int id = this->terrorClient->getId();
				string playerName = this->playerProfile->getUsername();
				GreetingsPtr greetings(new Greetings(playerName, id));
				this->terrorClient->sendObjectToHost(greetings, true);
			}
			if (udpPacket->getLabel() == GREETINGS) {
				GreetingsPtr greetings(new Greetings);
				if (!udpPacket->buildObject(greetings)) {
					return;
				}

				this->playerConnectedSound->stop();
				this->playerConnectedSound->play();
				std::string message = std::string("Player ") + greetings->getName() + std::string(" connected!");
				writeMessage(message);

				if (!this->terrorClient->isHost()) {
					return;
				}

				this->terrorClient->sendObjectToPlayers(greetings, true);

				PlayersStatePtr playersState = PlayersState::getInstance();
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(greetings->getId());
				if (!playerInfo) {
					return;
				}

				playerInfo->setName(greetings->getName());
				playerInfo->setUdpAddress(udpPacket->getRemoteUdpAddress());
				if (playerInfo->getCharacter() == CHAR_NONE) {
					playerInfo->setCharacter(CHAR_RANDOM);
				}
				playerInfo->updateGuiElements();

				this->terrorClient->sendObjectToPlayers(playersState, true);
			}
			if (udpPacket->getLabel() == PLAYERS_STATE) {
				PlayersStatePtr playersState = PlayersState::getInstance();
				if (!udpPacket->buildObject(playersState)) {
					return;
				}

				this->updateButtonsAvailability();

				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
				if (!playerInfo) {
					return;
				}

				CharacterType character = playerInfo->getCharacter();

				if (!this->ingame || character == CHAR_RANDOM || character == CHAR_NONE) {
					return;
				}

				if (isMonsterCharacter(character)) {
					MonsterControllerPtr monsterController = this->gameStateHandler->getMonsterController();
					if (!monsterController || monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
						return;
					}
					monsterController = this->characterLoader->transformMonsterControllerInLocal((this->gameStateHandler->getStage()->getName() == "Black_Lake_Woods"), 
						(this->gameStateHandler->getStage()->getName() == "Factory"));
					this->gameStateHandler->setMonsterController(monsterController);
					return;
				}

				if (this->gameStateHandler->humanExists(character)) {
					return;
				}

				if (!this->gameStateHandler->monsterExists()) {
					int monsterSpawnPoint = 4;
					CharacterType monsterChar = playersState->getMonsterCharacter();
					if (isMonsterCharacter(monsterChar)) {
						SpawnPointPtr spawnPoint = this->gameStateHandler->getStage()->getScene()->getSpawnPoint(monsterSpawnPoint);
						MonsterControllerPtr controller = this->characterLoader->loadRemotePlayerMonster(monsterChar, spawnPoint, (this->gameStateHandler->getStage()->getName() == "Black_Lake_Woods"), 
							(this->gameStateHandler->getStage()->getName() == "Factory"), this->gameStateHandler->getStage()->getName());
						this->gameStateHandler->setMonsterController(controller);
					}else {
						return;
					}
				}

				SpawnPointPtr spawnPoint = this->gameStateHandler->getStage()->getScene()->getSpawnPoint(playerInfo->getSpawnPointId());
				LocalPlayerHumanControllerPtr controller = this->characterLoader->loadLocalPlayerHuman(character, spawnPoint, (this->gameStateHandler->getStage()->getName() == "Black_Lake_Woods"));
				this->gameStateHandler->addHumanController(controller, true);				
			}
			if (udpPacket->getLabel() == CHAR_SELECTION_REQUEST) {
				CharSelectionRequestPtr charSelectionRequest(new CharSelectionRequest);
				if (!udpPacket->buildObject(charSelectionRequest)) {
					return;
				}

				int id = charSelectionRequest->getId();
				CharacterType character = charSelectionRequest->getCharacter();

				PlayersStatePtr playersState = PlayersState::getInstance();
				if (character != CHAR_RANDOM && playersState->existsCharacterPlayer(character)) {
					if (!isMonsterCharacter(character) || playersState->getMonsterPlayerId() != id) {
						return;
					}
				}

				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(id);
				if (!playerInfo) {
					return;
				}

				CharacterType oldCharacter = playerInfo->getCharacter();

				if (!isHumanCharacter(oldCharacter) && isHumanCharacter(character) && playersState->getNumberOfHumanPlayers() >= 4) {
					return;
				}
				
				playerInfo->setCharacter(character);
				playerInfo->updateGuiElements();

				this->updateButtonsAvailability();
				
				if (this->ingame) {
					if (character == CHAR_RANDOM) {
						playersState->resolveRandomCharacter(playerInfo);					
					}
					playersState->resolveSpawnPoint(playerInfo);
				}				
				this->terrorClient->sendObjectToPlayers(playersState, true);
			}

			if (udpPacket->getLabel() == PLAYER_JOINED_GAME) {
				GameSettingsPtr gameSettings = GameSettings::getInstance();
				if (this->terrorClient->isHost()) {
					this->terrorClient->sendObjectTo(gameSettings, udpPacket->getRemoteUdpAddress(), true);
				}
			}

			/*if (udpPacket->getLabel() == PLAYERS_STATE_ACK && this->ingame) {
				GameSettingsPtr gameSettings = GameSettings::getInstance();
				if (this->terrorClient->isHost()) {
					this->terrorClient->sendObjectTo(*gameSettings, udpPacket->getRemoteUdpAddress(), true);
				}
			}*/
		}

		void handleButton(Widget* button) {
			this->characterSelectionLayout->hide();
			this->lobbyLayout->enable();
			if (this->ingame) {
				MouseCursorPtr mouseCursor = MouseCursor::getInstance();
				mouseCursor->hide();
			}

			CharacterType character = CHAR_NONE;
			if (button == this->char0Button) {
				character = CHAR_0;
			}else if (button == this->char1Button) {
				character = CHAR_1;
			}else if (button == this->char2Button) {
				character = CHAR_2;
			}else if (button == this->char3Button) {
				character = CHAR_3;
			}else if (button == this->monster0Button) {
				character = CHAR_MONSTER_0;
			}else if (button == this->monster1Button) {
				character = CHAR_MONSTER_1;
			}else if (button == this->monster2Button) {
				character = CHAR_MONSTER_2;
			}else if (button == this->monster3Button) {
				character = CHAR_MONSTER_3;
			}else if (button == this->randomButton) {
				character = CHAR_RANDOM;
			}			

			int id = this->terrorClient->getId();
			if (!this->terrorClient->isHost()) {
				CharSelectionRequestPtr charSelectionRequest(new CharSelectionRequest(character, id));
				this->terrorClient->sendObjectToHost(charSelectionRequest, true);
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(id);
			if (!playerInfo) {
				return;
			}

			CharacterType oldCharacter = playerInfo->getCharacter();

			Widget* oldButton = this->getCharacterButton(oldCharacter);
			if (oldButton) {
				oldButton->setEnabled(true);
				if (isMonsterCharacter(oldCharacter)) {
					GameSettingsPtr gameSettings = GameSettings::getInstance();
					if (gameSettings->getMonsters()[SPECTRUM]) {
						this->monster0Button->setEnabled(true);
					}
					if (gameSettings->getMonsters()[BLIND]) {
						this->monster1Button->setEnabled(true);
					}
					if (gameSettings->getMonsters()[BERSERKER]) {
						this->monster2Button->setEnabled(true);
					}
					if (gameSettings->getMonsters()[STATUE]) {
						this->monster3Button->setEnabled(true);
					}
				}
			}
			playerInfo->setCharacter(character);
			playerInfo->updateGuiElements();
			Widget* newButton = this->getCharacterButton(character);
			if (newButton) {
				newButton->setEnabled(false);
			}

			this->terrorClient->sendObjectToPlayers(playersState, true);
		}

		void handleCancelButton(Widget* button) {
			this->lobbyLayout->enable();
			this->characterSelectionLayout->hide();
			if (this->ingame) {
				MouseCursorPtr mouseCursor = MouseCursor::getInstance();
				mouseCursor->hide();
			}
		}

		Widget* getCharacterButton(CharacterType character) {
			if (character == CHAR_NONE) {
				return 0;
			}			
			if (character == CHAR_0) {
				return this->char0Button;
			}
			if (character == CHAR_1) {
				return this->char1Button;
			}
			if (character == CHAR_2) {
				return this->char2Button;
			}
			if (character == CHAR_3) {
				return this->char3Button;
			}
			if (character == CHAR_MONSTER_0) {
				return this->monster0Button;
			}
			if (character == CHAR_MONSTER_1) {
				return this->monster1Button;
			}
			if (character == CHAR_MONSTER_2) {
				return this->monster2Button;			
			}
			if (character == CHAR_MONSTER_3) {
				return this->monster3Button;
			}
			if (character == CHAR_RANDOM) {
				return 0; //never disable the random button;
			}
			return 0;
		}

		void updateButtonsAvailability() {
			PlayersStatePtr playersState = PlayersState::getInstance();

			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}
			CharacterType character = playerInfo->getCharacter();
			
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getCharacterSelectionMode() == ALL_RANDOM_CHARACTERS) {
				this->char0Button->setEnabled(false);
				this->char1Button->setEnabled(false);
				this->char2Button->setEnabled(false);
				this->char3Button->setEnabled(false);
				this->monster0Button->setEnabled(false);
				this->monster1Button->setEnabled(false);
				this->monster2Button->setEnabled(false);
				this->monster3Button->setEnabled(false);
				return;
			}

			this->char0Button->setEnabled(true);
			this->char1Button->setEnabled(true);
			this->char2Button->setEnabled(true);
			this->char3Button->setEnabled(true);
			this->monster0Button->setEnabled(false);
			this->monster1Button->setEnabled(false);
			this->monster2Button->setEnabled(false);
			this->monster3Button->setEnabled(false);
			if (gameSettings->getMonsters()[SPECTRUM]) {
				this->monster0Button->setEnabled(true);
			}
			if (gameSettings->getMonsters()[BLIND]) {
				this->monster1Button->setEnabled(true);
			}
			if (gameSettings->getMonsters()[BERSERKER]) {
				this->monster2Button->setEnabled(true);
			}
			if (gameSettings->getMonsters()[STATUE]) {
				this->monster3Button->setEnabled(true);
			}

			for (int i = 0; i < 5; ++i) {
				PlayerInfoPtr playerInfoTmp = playersState->getPlayerInfo(i);
				if (playerInfoTmp) {
					Widget* button = this->getCharacterButton(playerInfoTmp->getCharacter());
					if (button) {
						button->setEnabled(false);
					}
				}
			}

			if (!isHumanCharacter(character) && playersState->getNumberOfHumanPlayers() >= 4) {
				this->char0Button->setEnabled(false);
				this->char1Button->setEnabled(false);
				this->char2Button->setEnabled(false);
				this->char3Button->setEnabled(false);
			}

			if (playersState->monsterPlayerExists()) {
				this->monster0Button->setEnabled(false);
				this->monster1Button->setEnabled(false);
				this->monster2Button->setEnabled(false);
				this->monster3Button->setEnabled(false);
				if (isMonsterCharacter(character)) {
					BoolVector& monstersAllowed = gameSettings->getMonsters();
					if (monstersAllowed[SPECTRUM]) {
						this->monster0Button->setEnabled(true);
					}
					if (monstersAllowed[BLIND]) {
						this->monster1Button->setEnabled(true);
					}
					if (monstersAllowed[BERSERKER]) {
						this->monster2Button->setEnabled(true);
					}
					if (monstersAllowed[STATUE]) {
						this->monster3Button->setEnabled(true);
					}
					if (character == CHAR_MONSTER_0) {
						this->monster0Button->setEnabled(false);
					}else if (character == CHAR_MONSTER_1) {
						this->monster1Button->setEnabled(false);
					}else if (character == CHAR_MONSTER_2) {
						this->monster2Button->setEnabled(false);
					}else if (character == CHAR_MONSTER_3) {
						this->monster3Button->setEnabled(false);
					}
				}
			}
		
			if (!this->ingame) {
				return;
			}

			if (gameSettings->getGameModeType() == NORMAL_MODE) {
				foreach (CharacterType& deadCharacter, playersState->getDeadCharacters()) {
					Widget* button = this->getCharacterButton(deadCharacter);
					if (button) {
						button->setEnabled(false);
					}
				}
			}			

			if (!playersState->monsterPlayerExists()) {
				this->monster0Button->setEnabled(false);
				this->monster1Button->setEnabled(false);
				this->monster2Button->setEnabled(false);
				this->monster3Button->setEnabled(false);
				if (playersState->getMonsterCharacter() == CHAR_MONSTER_0) {
					this->monster0Button->setEnabled(true);
				}else if (playersState->getMonsterCharacter() == CHAR_MONSTER_1) {
					this->monster1Button->setEnabled(true);
				}else if (playersState->getMonsterCharacter() == CHAR_MONSTER_2) {
					this->monster2Button->setEnabled(true);
				}else if (playersState->getMonsterCharacter() == CHAR_MONSTER_3) {
					this->monster3Button->setEnabled(true);
				}
			}
		}
	};

	typedef SharedPointer<CharacterSelectionWindowHandler> CharacterSelectionWindowHandlerPtr;
}

#endif
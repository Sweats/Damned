#ifndef TERROR_END_GAME_HANDLER_H
#define TERROR_END_GAME_HANDLER_H

#include "Packets/Packets.h"
#include "StageHandler/StageHandler.h"
#include "PauseHandler/PauseHandler.h"

namespace terror {
	class EndGameHandler : public NetworkObserver, public InputHandler {
	public:
		EndGameHandler(GuiLayoutPtr endGameLayout, GuiLayoutPtr lobbyLayout, GuiLayoutPtr chatLayout, GuiLayoutPtr loadingLayout, StageHandlerPtr stageHandler, 
			PauseHandlerPtr pauseHandler, TerrorClientPtr terrorClient) 
			: NetworkObserver(false), endGameLayout(endGameLayout), lobbyLayout(lobbyLayout), chatLayout(chatLayout), loadingLayout(loadingLayout), stageHandler(stageHandler), 
			pauseHandler(pauseHandler), terrorClient(terrorClient), timeToLobby(-1.0f), message("")
		{
			this->statusTextbox = static_cast<MyGUI::TextBox*>(this->endGameLayout->getWidget("MessageTextbox"));
		}

		void requestEndGame(EndGameStatus status) {
			EndGamePtr endGame(new EndGame(status));
			if (this->terrorClient->isHost()) {
				GameEndedPtr gameEnded(new GameEnded(status == MONSTER_WINS ? TEAM_1_WON : TEAM_2_WON));
				this->terrorClient->sendObject(gameEnded);
				this->terrorClient->sendObjectToPlayers(endGame, true);
			}
			this->handleEndGame(endGame);
		}

		void notifyStageLoaded(StagePtr stage) {
			this->stage = stage;
		}

		void notifyStageUnloaded() {
			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			mouseCursor->show();
			this->endGameLayout->hide();
			this->loadingLayout->hide();
			this->loadingLayout->setParent(GuiLayoutPtr());
			this->timeToLobby = -1.0f;
		}

		virtual ~EndGameHandler() {}

	protected:
		GuiLayoutPtr endGameLayout;
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr chatLayout;
		GuiLayoutPtr loadingLayout;
		MyGUI::TextBox* statusTextbox;
		StageHandlerPtr stageHandler;
		PauseHandlerPtr pauseHandler;
		TerrorClientPtr terrorClient;
		StagePtr stage;
		float timeToLobby;
		std::string message;

	private:
		virtual bool handleInput(float diffTime) {
			if (this->timeToLobby < 1.0e-4f) {
				return true;
			}
			this->timeToLobby -= diffTime;
			this->statusTextbox->setCaption(this->message + lexical_cast<std::string>(ceil(this->timeToLobby)));
			if (this->timeToLobby < 1.0e-4f) {
				this->timeToLobby = -1.0f;
				this->endGameLayout->hide();
				MyGUI::ImageBox* loadingImagebox = static_cast<MyGUI::ImageBox*>(this->loadingLayout->getWidget("LoadingImagebox"));
				loadingImagebox->setImageTexture(std::string("loading.jpg"));
				this->chatLayout->hide();
				this->loadingLayout->show(this->lobbyLayout);
				this->stageHandler->requestUnloadStage();
			}
			return true;
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == END_GAME) {
				EndGamePtr endGame(new EndGame);
				if (!udpPacket->buildObject(endGame)) {
					return;
				}
				this->handleEndGame(endGame);
			}
		}

		void handleEndGame(EndGamePtr endGame) {
			this->stageHandler->resetStage();
			Core::getInstance()->switchState("Posgame");
			this->timeToLobby = 10.0f;

			PlayerInfoPtr playerInfo = PlayersState::getInstance()->getPlayerInfo(this->terrorClient->getId());
			CharacterType characterType = CHAR_NONE;
			if (playerInfo) {
				characterType = playerInfo->getCharacter();
			}
			
			switch (endGame->status) {
				case GAME_CANCELED:
					this->message = "Game Canceled...";
					break;
				case MONSTER_WINS:
					this->message = "The monster killed everyone...";
					if (isMonsterCharacter(characterType)) {
						this->setSteamStatsAndAchivements(MONSTER_WINS);
					}
					break;
				case SURVIVORS_WINS:
					bool validCharacter = characterType != CHAR_NONE && characterType != CHAR_RANDOM;
					if (validCharacter && !isMonsterCharacter(characterType)) {
						this->setSteamStatsAndAchivements(SURVIVORS_WINS);
					}
					this->message = "The survivors escaped...";
					break;
			}
			this->endGameLayout->show();
			SteamSystem::getInstance()->uploadStats();
		}

		void setSteamStatsAndAchivements(EndGameStatus status) {
			SteamSystemPtr steamSystem = SteamSystem::getInstance();
			if (!steamSystem->isValid()) {
				return;
			}
			steamSystem->incrementStat("games_won", 1);
			if (!steamSystem->hasAchievement(stage->getName())) {
				steamSystem->incrementStat("stages_won", 1);
				steamSystem->setAchievement(stage->getName());
			}
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(terrorClient->getId());
			CharacterType localCharacter = CHAR_NONE;
			if (playerInfo) {
				localCharacter = playerInfo->getCharacter();
			}
			if (status == SURVIVORS_WINS) {
				if (PlayersState::getInstance()->getNumberOfLives() == 1) {
					steamSystem->setAchievement("life_zero");
				}
				GameSettingsPtr gameSettings = GameSettings::getInstance();
				if (gameSettings->getGameModeType() == NORMAL_MODE) {
					if (playersState->getDeadCharacters().size() == 3 && !playersState->isCharacterDead(localCharacter)) {
						steamSystem->setAchievement("last_man_standing");
					}
					if (playersState->getNumberOfHumanPlayers() == 4 && playersState->getDeadCharacters().size() == 0) {
						steamSystem->setAchievement("no_man_stands_alone");
					}
				}
			}else if (status == MONSTER_WINS) {
				if (isMonsterCharacter(localCharacter)) {
					steamSystem->setAchievement(this->getCharacterString(localCharacter));
				}
			}
		}

		string getCharacterString(CharacterType character) {
			switch (character) {
			case CHAR_MONSTER_0:
				return "Lurker";
				break;
			case CHAR_MONSTER_1:
				return "Phantom";
				break;
			case CHAR_MONSTER_2:
				return "Mary";
				break;
			case CHAR_MONSTER_3:
				return "Fallen";
				break;
			case CHAR_RANDOM:
			case CHAR_NONE:
				return "";
				break;
			}
			return "";
		}
	};

	typedef SharedPointer<EndGameHandler> EndGameHandlerPtr;
}

#endif
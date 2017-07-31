#ifndef TERROR_START_GAME_BUTTON_HANDLER_H
#define TERROR_START_GAME_BUTTON_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"
#include "StageHandler/StageHandler.h"

namespace terror {
	class StartGameButtonHandler : public ButtonHandler {
	public:
		StartGameButtonHandler(Widget* startGameButton, GuiLayoutPtr lobbyLayout, GuiLayoutPtr loadingLayout, 
			SoundPtr loadGameSound, TerrorClientPtr terrorClient, StageHandlerPtr stageHandler)
			: ButtonHandler(startGameButton), lobbyLayout(lobbyLayout), loadingLayout(loadingLayout), loadGameSound(loadGameSound), 
			terrorClient(terrorClient), stageHandler(stageHandler) {}

		void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == STAGE_INFO) {
				StageInfoPtr stageInfo(new StageInfo);
				if (!udpPacket->buildObject(stageInfo)) {
					return;
				}
				this->hideGui(stageInfo->name);
			}
		}

		virtual ~StartGameButtonHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr loadingLayout;
		SoundPtr loadGameSound;
		TerrorClientPtr terrorClient;
		StageHandlerPtr stageHandler;		

	private:
		virtual void handleButtonClick() {
			if (!this->terrorClient->isHost()) {
				return;
			}

			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getStagesIds().empty()) {
				writeMessage("Please add at least one stage to the Allowed Stages list (Game Settings)");
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			playersState->resolveRandomCharacters();
			playersState->resolveSpawnPoints();
			this->terrorClient->sendObjectToPlayers(playersState, true);
			
			int position = randomNumber(0, gameSettings->getStagesIds().size() - 1);
			StageDatabasePtr stageDatabase = StageDatabase::getInstance();
			string stageSelected = stageDatabase->getStageName(gameSettings->getStagesIds()[position]);
			replace_all(stageSelected, " ", "_");

			this->hideGui(stageSelected);
			this->stageHandler->requestLoadStage(stageSelected);
		}

		void hideGui(const std::string& stageName) {
			this->loadGameSound->stop();
			this->loadGameSound->play();
			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			mouseCursor->hide();
			this->lobbyLayout->hide(false);
			MyGUI::ImageBox* loadingImagebox = static_cast<MyGUI::ImageBox*>(this->loadingLayout->getWidget("LoadingImagebox"));
			std::string fileName = std::string("loading_") + stageName + std::string(".jpg");
			loadingImagebox->setImageTexture(fileName);
			this->loadingLayout->show();
		}
	};

	typedef SharedPointer<StartGameButtonHandler> StartGameButtonHandlerPtr;
}

#endif
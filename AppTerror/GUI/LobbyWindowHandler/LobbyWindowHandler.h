#ifndef TERROR_LOBBY_WINDOW_HANDLER_H
#define TERROR_LOBBY_WINDOW_HANDLER_H

#include "GUI/LobbyWindowHandler/StartGameButtonHandler/StartGameButtonHandler.h"
#include "GUI/LobbyWindowHandler/LeaveLobbyButtonHandler/LeaveLobbyButtonHandler.h"
#include "GUI/LobbyWindowHandler/KickButtonHandler/KickButtonHandler.h"
#include "GUI/LobbyWindowHandler/VoiceChatComboboxHandler/VoiceChatComboboxHandler.h" 
#include "GUI/LobbyWindowHandler/AllowSpectatorCheckboxHandler/AllowSpectatorCheckboxHandler.h"
#include "GUI/LobbyWindowHandler/AllowInsanityCheckboxHandler/AllowInsanityCheckboxHandler.h"
#include "GUI/LobbyWindowHandler/RandomCharacterSelectionCheckboxHandler/RandomCharacterSelectionCheckboxHandler.h"
#include "GUI/LobbyWindowHandler/StageSelectionComboboxHandler/StageSelectionComboboxHandler.h"
#include "GUI/LobbyWindowHandler/GameModeComboboxHandler/GameModeComboboxHandler.h"
#include "GUI/LobbyWindowHandler/NumberOfLivesComboboxHandler/NumberOfLivesComboboxHandler.h"
#include "GUI/CharacterSelectionWindowHandler/CharacterSelectionWindowHandler.h"

namespace terror {
	class LobbyWindowHandler : public NetworkObserver, public InputHandler {
	public:
		LobbyWindowHandler(GuiLayoutPtr lobbyLayout, GuiLayoutPtr chatLayout, GuiLayoutPtr characterSelectionLayout, GuiLayoutPtr loadingLayout, GuiLayoutPtr allowedMonstersLayout, 
			GuiLayoutPtr stageSelectionLayout, GuiLayoutPtr pauseLayout, StageSelectionWindowHandlerPtr stageSelectionWindowHandler, SoundPtr loadGameSound, TerrorClientPtr terrorClient, 
			StageHandlerPtr stageHandler, KickHandlerPtr kickHandler, TerrorSavePtr terrorSave)
			: NetworkObserver(false), lobbyLayout(lobbyLayout), stageSelectionLayout(stageSelectionLayout), loadingLayout(loadingLayout), terrorClient(terrorClient), terrorSave(terrorSave), ingame(false)
		{
			this->lobbyLayout->connectShowEvent(Bind(&LobbyWindowHandler::handleLayoutState, this));
			this->lobbyLayout->connectShowEvent(Bind(&GuiLayout::show, chatLayout));
			this->lobbyLayout->connectShowEvent(Bind(&GuiLayout::hide, pauseLayout, false));
			this->lobbyLayout->connectShowEvent(Bind(&GuiLayout::hide, characterSelectionLayout, false));
			this->lobbyLayout->connectHideEvent(Bind(&GuiLayout::hide, chatLayout, false));
			this->lobbyLayout->connectHideEvent(Bind(&GuiLayout::hide, characterSelectionLayout, false));
			this->lobbyLayout->connectHideEvent(Bind(&GuiLayout::hide, allowedMonstersLayout, false));
			this->lobbyLayout->connectHideEvent(Bind(&GuiLayout::hide, stageSelectionLayout, false));

			this->startGameButtonHandler = StartGameButtonHandlerPtr(
				new StartGameButtonHandler(lobbyLayout->getWidget("StartGameButton"), lobbyLayout, loadingLayout, loadGameSound, terrorClient, stageHandler)
				);
			this->characterSelectionButtonHandler = WindowCallerButtonHandlerPtr(
				new WindowCallerButtonHandler(lobbyLayout->getWidget("ChooseCharacterButton"), lobbyLayout, characterSelectionLayout, true)
				);

			this->allowedMonstersButtonHandler = WindowCallerButtonHandlerPtr(
				new WindowCallerButtonHandler(lobbyLayout->getWidget("AllowedMonstersButton"), lobbyLayout, allowedMonstersLayout, true)
				);
			this->voiceChatComboboxHandler = VoiceChatComboboxHandlerPtr(
				new VoiceChatComboboxHandler(lobbyLayout->getWidget("VoiceChatCombobox"), terrorClient)
				);
			this->allowSpectatorCheckboxHandler = AllowSpectatorCheckboxHandlerPtr(
				new AllowSpectatorCheckboxHandler(lobbyLayout->getWidget("AllowSpectatorCheckbox"), terrorClient)
				);
			this->allowInsanityCheckboxHandler = AllowInsanityCheckboxHandlerPtr(
				new AllowInsanityCheckboxHandler(lobbyLayout->getWidget("InsanityCheckbox"), terrorClient)
				);
			this->randomCharacterSelectionCheckboxHandler = RandomCharacterSelectionCheckboxHandlerPtr(
				new RandomCharacterSelectionCheckboxHandler(lobbyLayout->getWidget("RandomCharacterSelectionCheckbox"), terrorClient)
				);
			this->stageSelectionComboboxHandler = StageSelectionComboboxHandlerPtr(
				new StageSelectionComboboxHandler(lobbyLayout->getWidget("StageSelectionCombobox"), lobbyLayout, stageSelectionLayout, stageSelectionWindowHandler, terrorClient)
				);
			this->gameModeComboboxHandler = GameModeComboboxHandlerPtr(
				new GameModeComboboxHandler(lobbyLayout->getWidget("GameModeCombobox"), terrorClient)
				);
			this->numberOfLivesComboboxHandler = NumberOfLivesComboboxHandlerPtr(
				new NumberOfLivesComboboxHandler(lobbyLayout->getWidget("NumberOfLivesCombobox"), terrorClient)
				);

			this->voiceChatTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("VoiceChatTextbox"));
			this->allowSpectatorTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("AllowSpectatorTextbox"));
			this->allowInsanityTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("InsanityTextbox"));
			this->randomCharacterSelectionTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("RandomCharacterSelectionTextbox"));
			this->allowedMonstersListTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("AllowedMonstersListTextbox"));
			this->allowedMonstersTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("AllowedMonstersTextbox"));
			this->stageSelectionTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("StageSelectionTextbox"));
			this->allowedStagesTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("AllowedStagesTextbox"));
			this->gameModeTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("GameModeTextbox"));
			this->numberOfLivesTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("NumberOfLivesTextbox"));
			this->numberOfLivesTitleTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget("NumberOfLivesTitleTextbox"));
			this->stageSelectedImagebox = static_cast<MyGUI::ImageBox*>(lobbyLayout->getWidget("StageSelectedImagebox"));

			this->leaveLobbyButtonHandler = LeaveLobbyButtonHandlerPtr(
				new LeaveLobbyButtonHandler(lobbyLayout->getWidget("BackButton"), lobbyLayout, terrorClient)
				);

			PlayersStatePtr playersState = PlayersState::getInstance();
			for (int i = 0; i < 5; ++i) {
				string imageboxName = string("Player") + lexical_cast<string>(i) + string("Imagebox");
				MyGUI::ImageBox* playerImagebox = static_cast<MyGUI::ImageBox*>(lobbyLayout->getWidget(imageboxName));

				string textboxName = string("Player") + lexical_cast<string>(i) + string("NameTextbox");
				MyGUI::TextBox* playerNameTextbox = static_cast<MyGUI::TextBox*>(lobbyLayout->getWidget(textboxName));

				PlayerInfoPtr playerInfo(new PlayerInfo(playerImagebox, playerNameTextbox));
				playersState->addPlayerInfo(playerInfo);

				string kickButtonName = string("Player") + lexical_cast<string>(i) + string("KickButton");
				this->kickButtonHandlers[i] = KickButtonHandlerPtr(new KickButtonHandler(lobbyLayout->getWidget(kickButtonName), this->terrorClient, kickHandler, i));
			}			
		}

		void notifyGameSettingsUpdated() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->allowedMonstersListTextbox->setCaption(this->getAllowedMonstersText());
			this->allowSpectatorTextbox->setCaption(gameSettings->isSpectatorAllowed() ? std::string("Yes") : std::string("No"));
			this->allowInsanityTextbox->setCaption(gameSettings->isInsanityAllowed() ? std::string("Yes") : std::string("No"));
			bool randomCharacterSelectionMode = gameSettings->getCharacterSelectionMode() == ALL_RANDOM_CHARACTERS;
			this->lobbyLayout->getWidget("ChooseCharacterButton")->setVisible(!randomCharacterSelectionMode);
			this->randomCharacterSelectionTextbox->setCaption((randomCharacterSelectionMode) ? std::string("Yes") : std::string("No"));
			this->voiceChatTextbox->setCaption(toVoiceChatModeText(gameSettings->getVoiceChatMode()));
			this->stageSelectionTextbox->setCaption(toStageSelectionModeText(gameSettings->getStageSelectionMode()));
			this->gameModeTextbox->setCaption(toGameModeText(gameSettings->getGameModeType()));
			this->updateNumberOfLivesWidgetsVisibility();
			this->numberOfLivesTextbox->setCaption(lexical_cast<string>(gameSettings->getNumberOfLives()));
			this->updateAllowedStagesTextbox();
			this->updateStageSelectedImagebox();
		}

		void notifyStageLoaded(StagePtr stage) {
			this->loadingLayout->hide();
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->ingame = false;
		}

		virtual ~LobbyWindowHandler() {}

	protected:
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr stageSelectionLayout;
		GuiLayoutPtr loadingLayout;
		StartGameButtonHandlerPtr startGameButtonHandler;
		WindowCallerButtonHandlerPtr characterSelectionButtonHandler;
		WindowCallerButtonHandlerPtr allowedMonstersButtonHandler;
		VoiceChatComboboxHandlerPtr voiceChatComboboxHandler;
		LeaveLobbyButtonHandlerPtr leaveLobbyButtonHandler;
		RandomCharacterSelectionCheckboxHandlerPtr randomCharacterSelectionCheckboxHandler;
		KickButtonHandlerPtr kickButtonHandlers[5];
		AllowSpectatorCheckboxHandlerPtr allowSpectatorCheckboxHandler;
		AllowInsanityCheckboxHandlerPtr allowInsanityCheckboxHandler;
		StageSelectionComboboxHandlerPtr stageSelectionComboboxHandler;
		GameModeComboboxHandlerPtr gameModeComboboxHandler;
		NumberOfLivesComboboxHandlerPtr numberOfLivesComboboxHandler;
		MyGUI::TextBox* voiceChatTextbox;
		MyGUI::TextBox* allowSpectatorTextbox;
		MyGUI::TextBox* allowInsanityTextbox;
		MyGUI::TextBox* randomCharacterSelectionTextbox;
		MyGUI::TextBox* allowedMonstersListTextbox;
		MyGUI::TextBox* allowedMonstersTextbox;
		MyGUI::TextBox* stageSelectionTextbox;
		MyGUI::TextBox* allowedStagesTextbox;
		MyGUI::TextBox* gameModeTextbox;
		MyGUI::TextBox* numberOfLivesTextbox;
		MyGUI::TextBox* numberOfLivesTitleTextbox;
		MyGUI::ImageBox* stageSelectedImagebox;
		bool ingame;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			for (int i = 0; i < 5; ++i) {
				this->kickButtonHandlers[i]->handleTcpPacket(tcpPacket);
			}
			if (this->ingame) {
				return;
			}
			if (tcpPacket->getLabel() == HOST_INFO) {
				this->startGameButtonHandler->showButton();
				this->allowedMonstersButtonHandler->showButton();
				this->allowedMonstersTextbox->setVisible(true);
				this->voiceChatTextbox->setVisible(false);
				this->lobbyLayout->getWidget("VoiceChatCombobox")->setVisible(true);
				this->allowSpectatorTextbox->setVisible(false);
				this->lobbyLayout->getWidget("AllowSpectatorCheckbox")->setVisible(true);
				this->allowInsanityTextbox->setVisible(false);
				this->lobbyLayout->getWidget("InsanityCheckbox")->setVisible(true);
				this->randomCharacterSelectionTextbox->setVisible(false);
				this->lobbyLayout->getWidget("RandomCharacterSelectionCheckbox")->setVisible(true);
				this->stageSelectionTextbox->setVisible(false);
				this->lobbyLayout->getWidget("StageSelectionCombobox")->setVisible(true);
				this->gameModeTextbox->setVisible(false);
				this->lobbyLayout->getWidget("GameModeCombobox")->setVisible(true);
				this->updateNumberOfLivesWidgetsVisibility();
				this->updateBasedOnSettings();
				for (int i = 0; i < 5; ++i) {
					this->kickButtonHandlers[i]->reset();
				}
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			for (int i = 0; i < 5; ++i) {
				this->kickButtonHandlers[i]->handleUdpPacket(udpPacket);
			}
			if (udpPacket->getLabel() == GREETINGS) {
				if (!this->terrorClient->isHost() || this->ingame) {
					return;
				}
				GameSettingsPtr gameSettings = GameSettings::getInstance();
				this->terrorClient->sendObjectToPlayers(gameSettings, true);
			}
			if (udpPacket->getLabel() == GAME_SETTINGS) {
				GameSettingsPtr gameSettings = GameSettings::getInstance();
				udpPacket->buildObject(gameSettings);
			}
			if (this->ingame) {
				return;
			}
			this->startGameButtonHandler->handleUdpPacket(udpPacket);
		}

		void handleLayoutState() {
			if (this->terrorClient->isHost()) {
				this->startGameButtonHandler->showButton();
				this->allowedMonstersButtonHandler->showButton();
				this->allowedMonstersTextbox->setVisible(true);
				this->voiceChatTextbox->setVisible(false);
				this->lobbyLayout->getWidget("VoiceChatCombobox")->setVisible(true);
				this->allowSpectatorTextbox->setVisible(false);
				this->lobbyLayout->getWidget("AllowSpectatorCheckbox")->setVisible(true);
				this->allowInsanityTextbox->setVisible(false);
				this->lobbyLayout->getWidget("InsanityCheckbox")->setVisible(true);				
				this->randomCharacterSelectionTextbox->setVisible(false);
				this->lobbyLayout->getWidget("RandomCharacterSelectionCheckbox")->setVisible(true);
				this->stageSelectionTextbox->setVisible(false);
				this->lobbyLayout->getWidget("StageSelectionCombobox")->setVisible(true);
				this->gameModeTextbox->setVisible(false);
				this->lobbyLayout->getWidget("GameModeCombobox")->setVisible(true);
				this->updateNumberOfLivesWidgetsVisibility();
				this->updateBasedOnSettings();
				this->notifyGameSettingsUpdated();
				for (int i = 0; i < 5; ++i) {
					this->kickButtonHandlers[i]->update();
				}
			}else {
				this->startGameButtonHandler->hideButton();
				this->allowedMonstersButtonHandler->hideButton();
				this->allowedMonstersTextbox->setVisible(true);
				this->voiceChatTextbox->setVisible(true);
				this->lobbyLayout->getWidget("VoiceChatCombobox")->setVisible(false);
				this->allowSpectatorTextbox->setVisible(true);
				this->lobbyLayout->getWidget("AllowSpectatorCheckbox")->setVisible(false);
				this->allowInsanityTextbox->setVisible(true);
				this->lobbyLayout->getWidget("InsanityCheckbox")->setVisible(false);				
				this->randomCharacterSelectionTextbox->setVisible(true);
				this->lobbyLayout->getWidget("RandomCharacterSelectionCheckbox")->setVisible(false);
				this->stageSelectionTextbox->setVisible(true);
				this->lobbyLayout->getWidget("StageSelectionCombobox")->setVisible(false);
				this->gameModeTextbox->setVisible(true);
				this->lobbyLayout->getWidget("GameModeCombobox")->setVisible(false);
				this->updateNumberOfLivesWidgetsVisibility();
				this->notifyGameSettingsUpdated();
				for (int i = 0; i < 5; ++i) {
					string kickButtonName = string("Player") + lexical_cast<string>(i) + "KickButton";
					this->lobbyLayout->getWidget(kickButtonName)->setVisible(false);
				}
			}
		}

		std::string getAllowedMonstersText() {
			std::string allowedMonstersText;
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			BoolVector& monsters = gameSettings->getMonsters(); 
			bool firstMonster = true;
			for (int i = 0; i < N_MONSTERS; ++i) {
				if (monsters[i]) {
					if (firstMonster) {
						allowedMonstersText += toMonsterTypeText(i);
						firstMonster = false;
					}else {
						allowedMonstersText += std::string("\n") + toMonsterTypeText(i);
					}
				}
			}
			return allowedMonstersText;
		}

		std::string getAllowedStagesText() {
			StageDatabasePtr stageDatabase = StageDatabase::getInstance();
			std::string allowedStagesText("Allowed Stages:");
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			IntVector& stagesIds = gameSettings->getStagesIds();
			foreach (int stageId, stagesIds) {
				allowedStagesText += std::string("\n\t") + stageDatabase->getStageName(stageId);
			}
			return allowedStagesText;
		}

		void updateBasedOnSettings() {
			this->voiceChatComboboxHandler->updateBasedOnSettings();
			this->allowSpectatorCheckboxHandler->updateBasedOnSettings();
			this->allowInsanityCheckboxHandler->updateBasedOnSettings();
			this->randomCharacterSelectionCheckboxHandler->updateBasedOnSettings();
			this->stageSelectionComboboxHandler->updateBasedOnSettings();
			this->gameModeComboboxHandler->updateBasedOnSettings();
			this->numberOfLivesComboboxHandler->updateBasedOnSettings();
		}

		void updateAllowedStagesTextbox() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getStageSelectionMode() == RANDOM) {
				this->allowedStagesTextbox->setVisible(true);
				this->allowedStagesTextbox->setCaption(this->getAllowedStagesText());
			}else {
				this->allowedStagesTextbox->setVisible(false);
			}
		}

		void updateStageSelectedImagebox() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getStageSelectionMode() == CHOOSE_ONE) {
				this->stageSelectedImagebox->setVisible(false);
				IntVector& stagesIds = GameSettings::getInstance()->getStagesIds();
				if (stagesIds.size() == 1) {
					this->stageSelectedImagebox->setVisible(true);
					int stageId = stagesIds[0];
					this->stageSelectedImagebox->setImageTexture(std::string("stage_") + lexical_cast<string>(stageId) + std::string(".png"));
				}
			}else {
				this->stageSelectedImagebox->setVisible(false);
			}
		}

		void updateNumberOfLivesWidgetsVisibility() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getGameModeType() != LIFE_MODE) {
				this->numberOfLivesTextbox->setVisible(false);
				this->lobbyLayout->getWidget("NumberOfLivesCombobox")->setVisible(false);
				this->numberOfLivesTitleTextbox->setVisible(false);
				return;
			}
			this->numberOfLivesTitleTextbox->setVisible(true);
			if (this->terrorClient->isHost()) {
				this->numberOfLivesTextbox->setVisible(false);
				this->lobbyLayout->getWidget("NumberOfLivesCombobox")->setVisible(true);
			}else {
				this->numberOfLivesTextbox->setVisible(true);
				this->lobbyLayout->getWidget("NumberOfLivesCombobox")->setVisible(false);
			}
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->lobbyLayout->isVisible()) {
				LeaveGameRequestPtr leaveGameRequest(new LeaveGameRequest);
				this->terrorClient->sendObject(leaveGameRequest);
				this->terrorClient->reset();
				this->lobbyLayout->hide();
			}
		}
	};

	typedef SharedPointer<LobbyWindowHandler> LobbyWindowHandlerPtr;
}

#endif
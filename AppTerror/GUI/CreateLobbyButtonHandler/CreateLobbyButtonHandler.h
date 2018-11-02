#ifndef TERROR_CREATE_LOBBY_BUTTON_HANDLER_H
#define TERROR_CREATE_LOBBY_BUTTON_HANDLER_H

#include "Config.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	class CreateLobbyButtonHandler : public ButtonHandler {
	public:
		CreateLobbyButtonHandler(Widget* createLobbyButton, MyGUI::EditBox* gameNameEditbox, MyGUI::EditBox* gamePasswordEditbox,
			GuiLayoutPtr hostLayout, GuiLayoutPtr lobbyLayout, TerrorClientPtr terrorClient)
			: ButtonHandler(createLobbyButton), gameNameEditbox(gameNameEditbox), gamePasswordEditbox(gamePasswordEditbox), 
			hostLayout(hostLayout), lobbyLayout(lobbyLayout), terrorClient(terrorClient) 
		{
			this->gameNameEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &CreateLobbyButtonHandler::handleAccept);
			this->gamePasswordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &CreateLobbyButtonHandler::handleAccept);
		}

		virtual ~CreateLobbyButtonHandler() {}

	protected:
		GuiLayoutPtr hostLayout;
		GuiLayoutPtr lobbyLayout;
		MyGUI::EditBox* gameNameEditbox;
		MyGUI::EditBox* gamePasswordEditbox;
		TerrorClientPtr terrorClient;

	private:
		virtual void handleButtonClick() {
			string name = this->gameNameEditbox->getOnlyText();
			if (name == "") {
				return;
			}
			// PUBLIC TEST PATCH NAMETAG ADDED IN LOBBY HEADER
			name = "#FF0000[PTP v001]#FFFFFF"+name;
			string password = this->gamePasswordEditbox->getOnlyText();
			GameCreationRequestPtr gameCreationRequest(new GameCreationRequest(name, password, 5, 5));
			this->terrorClient->sendObject(gameCreationRequest);
			this->hostLayout->hide(false);
			this->lobbyLayout->show();
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->reset();
			gameSettings->notifyUpdate();
		}

		void handleAccept(Widget* widget) {
			this->handleButtonClick();
		}
	};

	typedef SharedPointer<CreateLobbyButtonHandler> CreateLobbyButtonHandlerPtr;

	class HostGameWindowHandler : public InputHandler {
	public:
		HostGameWindowHandler(GuiLayoutPtr hostLayout, GuiLayoutPtr lobbyLayout, GuiLayoutPtr chatLayout, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave)
			: hostLayout(hostLayout), terrorSave(terrorSave)
		{
			MyGUI::EditBox* gameNameEditbox = static_cast<MyGUI::EditBox*>(hostLayout->getWidget("GameSessionNameEditbox"));
			MyGUI::EditBox* gamePasswordEditbox = static_cast<MyGUI::EditBox*>(hostLayout->getWidget("GameSessionPasswordEditbox"));

			this->createLobbyButtonHandler =
				CreateLobbyButtonHandlerPtr(new CreateLobbyButtonHandler(hostLayout->getWidget("StartGameSessionButton"), gameNameEditbox, gamePasswordEditbox,
				hostLayout, lobbyLayout, terrorClient));
			this->backButtonHandler =
				BackButtonHandlerPtr(new BackButtonHandler(hostLayout->getWidget("BackButton"), hostLayout));
		}

		virtual ~HostGameWindowHandler() {}

	protected:
		GuiLayoutPtr hostLayout;
		CreateLobbyButtonHandlerPtr createLobbyButtonHandler;
		BackButtonHandlerPtr backButtonHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->hostLayout->isVisible()) {
				this->hostLayout->hide();
			}
		}
	};
	typedef SharedPointer<HostGameWindowHandler> HostGameWindowHandlerPtr;
}

#endif
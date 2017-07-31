#ifndef TERROR_JOIN_GAME_WINDOW_PASSWORD_HANDLER_H
#define TERROR_JOIN_GAME_WINDOW_PASSWORD_HANDLER_H

#include "GUI/JoinGameWindowHandler/ConnectButtonHandler/ConnectButtonHandler.h"

namespace terror {
	class JoinGamePasswordWindowHandler {
	public:
		JoinGamePasswordWindowHandler(GuiLayoutPtr joinLayout, GuiLayoutPtr gamePasswordLayout, TerrorClientPtr terrorClient) 
			: joinLayout(joinLayout), gamePasswordLayout(gamePasswordLayout)
		{
			Widget* connectButton = gamePasswordLayout->getWidget("ConnectButton");
			MyGUI::EditBox* passwordEditbox = static_cast<MyGUI::EditBox*>(this->gamePasswordLayout->getWidget("PasswordEditbox"));
			this->connectButtonHandler = ConnectButtonHandlerPtr(
				new ConnectButtonHandler(connectButton, passwordEditbox, gamePasswordLayout, terrorClient));

			this->backButtonHandler = BackButtonHandlerPtr(
				new BackButtonHandler(gamePasswordLayout->getWidget("CancelButton"), gamePasswordLayout));
		}

		void connectHideEvent(const BasicSignal::slot_type& hideEvent) {
			this->gamePasswordLayout->connectHideEvent(hideEvent);
		}

		void openPasswordWindow(int gameSessionId) {
			this->connectButtonHandler->setGameSessionId(gameSessionId);			
			this->joinLayout->hide(false);
			this->gamePasswordLayout->show(this->joinLayout);
		}

		virtual ~JoinGamePasswordWindowHandler() {}

	protected:
		GuiLayoutPtr joinLayout;
		GuiLayoutPtr gamePasswordLayout;
		ConnectButtonHandlerPtr connectButtonHandler;
		BackButtonHandlerPtr backButtonHandler;
	};
	typedef SharedPointer<JoinGamePasswordWindowHandler> JoinGamePasswordWindowHandlerPtr;
}

#endif
#ifndef TERROR_CONNECT_BUTTON_HANDLER_H
#define TERROR_CONNECT_BUTTON_HANDLER_H

#include "MessageSystem/MessageSystem.h"

namespace terror {
	class ConnectButtonHandler : public ButtonHandler {
	public:
		ConnectButtonHandler(Widget* connectButton, MyGUI::EditBox* passwordEditbox, GuiLayoutPtr gamePasswordLayout, TerrorClientPtr terrorClient) 
			: ButtonHandler(connectButton), passwordEditbox(passwordEditbox), gamePasswordLayout(gamePasswordLayout), 
			terrorClient(terrorClient), gameSessionId(-1) 
		{
			this->passwordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ConnectButtonHandler::handleAccept);
		}

		void setGameSessionId(int gameSessionId) {
			this->gameSessionId = gameSessionId;
		}

		virtual ~ConnectButtonHandler() {}

	protected:		
		MyGUI::EditBox* passwordEditbox;
		GuiLayoutPtr gamePasswordLayout;
		TerrorClientPtr terrorClient;
		int gameSessionId;

	private:
		virtual void handleButtonClick() {
			std::string password = this->passwordEditbox->getOnlyText();
			if (password == "") {
				return;
			}
			JoinGameRequestPtr joinGameRequest(new JoinGameRequest(this->gameSessionId, password));
			this->terrorClient->sendObject(joinGameRequest);
			this->gamePasswordLayout->hide();
		}

		void handleAccept(Widget* widget) {
			this->handleButtonClick();
		}
	};
	typedef SharedPointer<ConnectButtonHandler> ConnectButtonHandlerPtr;
}

#endif
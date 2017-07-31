#ifndef TERROR_JOIN_GAME_BUTTON_HANDLER_H
#define TERROR_JOIN_GAME_BUTTON_HANDLER_H

#include "GUI/JoinGameWindowHandler/JoinGamePasswordWindowHandler.h"

namespace terror {
	class JoinGameButtonHandler : public ButtonHandler {
	public:
		JoinGameButtonHandler(Widget* joinGameButton, MyGUI::MultiListBox* gameSessionsListbox, JoinGamePasswordWindowHandlerPtr joinGamePasswordWindowHandler,
			TerrorClientPtr terrorClient) 
			: ButtonHandler(joinGameButton), joinGameButton(joinGameButton), gameSessionsListbox(gameSessionsListbox), 
			joinGamePasswordWindowHandler(joinGamePasswordWindowHandler), terrorClient(terrorClient) 
		{
			this->joinGamePasswordWindowHandler->connectHideEvent(Bind(&JoinGameButtonHandler::enableJoinButton, this));
		}

		void disableJoinButton() {
			this->joinGameButton->setEnabled(false);
		}

		void enableJoinButton() {
			this->joinGameButton->setEnabled(true);
		}

		virtual ~JoinGameButtonHandler() {}

	protected:
		Widget* joinGameButton;
		MyGUI::MultiListBox* gameSessionsListbox;
		JoinGamePasswordWindowHandlerPtr joinGamePasswordWindowHandler;
		TerrorClientPtr terrorClient;

	private:
		virtual void handleButtonClick() {
			if (this->gameSessionsListbox->getIndexSelected() == MyGUI::ITEM_NONE) {
				return;
			}
			bool isPublic = this->gameSessionsListbox->getSubItemNameAt(3, this->gameSessionsListbox->getIndexSelected()) == "Yes" ? true : false;
			int id = lexical_cast<int>(this->gameSessionsListbox->getSubItemNameAt(0, this->gameSessionsListbox->getIndexSelected()));
			if (isPublic) {				
				this->gameSessionsListbox->setEnabled(false);
				this->disableJoinButton();
				JoinGameRequestPtr joinGameRequest(new JoinGameRequest(id, ""));
				this->terrorClient->sendObject(joinGameRequest);
			}else {
				this->joinGamePasswordWindowHandler->openPasswordWindow(id);
			}
		}
	};	
	typedef SharedPointer<JoinGameButtonHandler> JoinGameButtonHandlerPtr;
}

#endif
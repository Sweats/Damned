#ifndef TERROR_JOIN_GAME_WINDOW_HANDLER_H
#define TERROR_JOIN_GAME_WINDOW_HANDLER_H

#include "GUI/JoinGameWindowHandler/JoinGameButtonHandler/JoinGameButtonHandler.h"

namespace terror {
	class JoinGameWindowHandler : public NetworkObserver, public InputHandler {
	public:
		JoinGameWindowHandler(GuiLayoutPtr joinLayout, GuiLayoutPtr gamePasswordLayout, GuiLayoutPtr lobbyLayout, GuiLayoutPtr chatLayout, 
			TerrorClientPtr terrorClient, TerrorSavePtr terrorSave) 
			: NetworkObserver(false), joinLayout(joinLayout), lobbyLayout(lobbyLayout), chatLayout(chatLayout), terrorClient(terrorClient), terrorSave(terrorSave), ingame(false)
		{
			joinLayout->connectShowEvent(Bind(&JoinGameWindowHandler::sendGameSessionListRequest, this));

			this->gameSessionsListbox = static_cast<MyGUI::MultiListBox*>(joinLayout->getWidget("GameSessionsMultiList"));
			this->gameSessionsListbox->eventListSelectAccept += MyGUI::newDelegate(this, &JoinGameWindowHandler::handleGameSessionsItemSelected);

			this->joinGamePasswordWindowHandler = JoinGamePasswordWindowHandlerPtr(
				new JoinGamePasswordWindowHandler(joinLayout, gamePasswordLayout, terrorClient));

			this->joinGameButtonHandler = JoinGameButtonHandlerPtr(
				new JoinGameButtonHandler(joinLayout->getWidget("SelectHostButton"), gameSessionsListbox, this->joinGamePasswordWindowHandler, terrorClient));
			this->backButtonHandler = BackButtonHandlerPtr(
				new BackButtonHandler(joinLayout->getWidget("BackButton"), joinLayout));	
		}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->ingame = false;
		}

		virtual ~JoinGameWindowHandler() {}

	protected:
		GuiLayoutPtr joinLayout;
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr chatLayout;
		MyGUI::MultiListBox* gameSessionsListbox;
		JoinGamePasswordWindowHandlerPtr joinGamePasswordWindowHandler;
		JoinGameButtonHandlerPtr joinGameButtonHandler;
		BackButtonHandlerPtr backButtonHandler;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		bool ingame;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == JOIN_INFO) {
				if (this->ingame) {
					return;
				}
				this->joinLayout->hide(false);
				this->lobbyLayout->show();
				this->gameSessionsListbox->setEnabled(true);
				this->joinGameButtonHandler->enableJoinButton();
			}

			if (tcpPacket->getLabel() == JOIN_ERROR_RESPONSE) {
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse);
				tcpPacket->buildObject(joinErrorResponse);
				if (joinErrorResponse->getJoinError() == JOIN_HOST_TIMEOUT) {
					writeMessage("Host timed out.");
				}else if (joinErrorResponse->getJoinError() == JOIN_INVALID_PASSWORD) {
					writeMessage("Invalid password.");
				}else if (joinErrorResponse->getJoinError() == JOIN_HOST_FULL) {
					writeMessage("Game session is full.");
				}
				this->gameSessionsListbox->setEnabled(true);
				this->joinGameButtonHandler->enableJoinButton();
			}
		}

		void sendGameSessionListRequest() {
			GameSessionListRequestPtr gameSessionListRequest(new GameSessionListRequest);
			this->terrorClient->sendObject(gameSessionListRequest);
		}

		void handleGameSessionsItemSelected(MyGUI::MultiListBox* sender, size_t index) {
			if (this->gameSessionsListbox->getIndexSelected() == MyGUI::ITEM_NONE) {
				return;
			}
			bool isPublic = this->gameSessionsListbox->getSubItemNameAt(3, this->gameSessionsListbox->getIndexSelected()) == "Yes" ? true : false;
			int id = lexical_cast<int>(this->gameSessionsListbox->getSubItemNameAt(0, this->gameSessionsListbox->getIndexSelected()));			
			if (isPublic) {				
				this->gameSessionsListbox->setEnabled(false);
				this->joinGameButtonHandler->disableJoinButton();
				JoinGameRequestPtr joinGameRequest(new JoinGameRequest(id, ""));
				this->terrorClient->sendObject(joinGameRequest);				
			}else {
				this->joinGamePasswordWindowHandler->openPasswordWindow(id);
			}
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->joinLayout->isVisible()) {
				this->joinLayout->hide();
			}
		}
	};
	typedef SharedPointer<JoinGameWindowHandler> JoinGameWindowHandlerPtr;
}

#endif
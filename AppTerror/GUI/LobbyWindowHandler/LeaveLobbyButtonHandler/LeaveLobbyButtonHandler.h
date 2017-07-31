#ifndef TERROR_LEAVE_LOBBY_BUTTON_HANDLER_H
#define TERROR_LEAVE_LOBBY_BUTTON_HANDLER_H

#include "Config.h"

namespace terror {
	class LeaveLobbyButtonHandler : public ButtonHandler {
	public:
		LeaveLobbyButtonHandler(Widget* leaveLobbyButton, GuiLayoutPtr lobbyLayout, TerrorClientPtr terrorClient) 
			: ButtonHandler(leaveLobbyButton), lobbyLayout(lobbyLayout), terrorClient(terrorClient) {}

		virtual ~LeaveLobbyButtonHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		TerrorClientPtr terrorClient;

	private:
		virtual void handleButtonClick() {
			LeaveGameRequestPtr leaveGameRequest(new LeaveGameRequest);
			this->terrorClient->sendObject(leaveGameRequest);
			this->terrorClient->reset();
			this->lobbyLayout->hide();
		}
	};
	typedef SharedPointer<LeaveLobbyButtonHandler> LeaveLobbyButtonHandlerPtr;
}

#endif
#ifndef TERROR_LOGOFF_BUTTON_HANDLER_H
#define TERROR_LOGOFF_BUTTON_HANDLER_H

#include "LoginSaver/LoginSaver.h"
#include "PlayerProfile/PlayerProfile.h"

namespace terror {
	class LogoffButtonHandler : public ButtonHandler {
	public:
		LogoffButtonHandler(GuiLayoutPtr mainLayout, GuiLayoutPtr loginLayout, LoginSaverPtr loginSaver, PlayerProfilePtr playerProfile, TerrorClientPtr terrorClient)
			: ButtonHandler(mainLayout->getWidget("LogoffButton")), mainLayout(mainLayout), loginLayout(loginLayout), loginSaver(loginSaver), playerProfile(playerProfile), 
			terrorClient(terrorClient) {}

		virtual ~LogoffButtonHandler(){}

	protected:
		GuiLayoutPtr mainLayout;
		GuiLayoutPtr loginLayout;
		LoginSaverPtr loginSaver;
		PlayerProfilePtr playerProfile;
		TerrorClientPtr terrorClient;

	private:
		virtual void handleButtonClick() {
			this->loginSaver->clear();
			this->mainLayout->hide();
			this->playerProfile->reset();
			LogoffRequestPtr logoffRequest(new LogoffRequest);
			this->terrorClient->sendObject(logoffRequest);
			this->loginLayout->show();
		}
	};

	typedef SharedPointer<LogoffButtonHandler> LogoffButtonHandlerPtr;
}

#endif
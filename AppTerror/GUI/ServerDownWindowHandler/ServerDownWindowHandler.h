#ifndef TERROR_SERVER_DOWN_WINDOW_HANDLER_H
#define TERROR_SERVER_DOWN_WINDOW_HANDLER_H

#include "ExitTrigger/ExitTrigger.h"

namespace terror {
	class ServerDownWindowHandler {
	public:
		ServerDownWindowHandler(GuiLayoutPtr loginLayout, ExitTriggerPtr exitTrigger)
			: exitTrigger(exitTrigger)
		{
			Widget* loginButton = loginLayout->getWidget("LoginButton");
			loginButton->setEnabled(false);

			Widget* registerButton = loginLayout->getWidget("RegisterButton");
			registerButton->setEnabled(false);

			Widget* resetPasswordButton = loginLayout->getWidget("ForgotButton");
			resetPasswordButton->setEnabled(false);

			MyGUI::EditBox* usernameOrEmailEditbox = static_cast<MyGUI::EditBox*>(loginLayout->getWidget("UsernameOrEmailEditbox"));
			usernameOrEmailEditbox->setEnabled(false);

			MyGUI::EditBox* passwordEditbox = static_cast<MyGUI::EditBox*>(loginLayout->getWidget("PasswordEditbox"));
			passwordEditbox->setEnabled(false);

			Widget* exitButton = loginLayout->getWidget("ExitButton");
			exitButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ServerDownWindowHandler::handleExitClick);

			MyGUI::TextBox* errorMessageTextbox = static_cast<MyGUI::TextBox*>(loginLayout->getWidget("ErrorMessageTextbox"));
			errorMessageTextbox->setCaption("Server is currently down for maintenance...");
		}

		virtual ~ServerDownWindowHandler() {}

	protected:
		ExitTriggerPtr exitTrigger;

	private:
		void handleExitClick(Widget* button) {
			this->exitTrigger->exit();
		}
	};

	typedef SharedPointer<ServerDownWindowHandler> ServerDownWindowHandlerPtr;
}

#endif
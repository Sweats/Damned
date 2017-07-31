#ifndef TERROR_LOGIN_WINDOW_HANDLER_H
#define TERROR_LOGIN_WINDOW_HANDLER_H

#include "ExitTrigger/ExitTrigger.h"
#include "PlayerProfile/PlayerProfile.h"
#include "LoginSaver/LoginSaver.h"
#include "RememberMeCheckboxHandler/RememberMeCheckboxHandler.h"

namespace terror {
	class LoginWindowHandler : public NetworkObserver {
	public:
		LoginWindowHandler(GuiLayoutPtr loginLayout, GuiLayoutPtr mainLayout, GuiLayoutPtr activationLayout, GuiLayoutPtr registerLayout, 
			GuiLayoutPtr resetPasswordLayout, GuiLayoutPtr optionsLayout, TerrorClientPtr terrorClient, PlayerProfilePtr playerProfile, ExitTriggerPtr exitTrigger, LoginSaverPtr loginSaver)
			: NetworkObserver(false), loginLayout(loginLayout), mainLayout(mainLayout), activationLayout(activationLayout), registerLayout(registerLayout), optionsLayout(optionsLayout), 
			terrorClient(terrorClient), playerProfile(playerProfile), exitTrigger(exitTrigger), loginSaver(loginSaver)
		{
			this->loginLayout->connectShowEvent(Bind(&LoginWindowHandler::handleLayoutState, this));

			Widget* loginButton = loginLayout->getWidget("LoginButton");
			loginButton->eventMouseButtonClick += MyGUI::newDelegate(this, &LoginWindowHandler::handleLoginClick);

			Widget* registerButton = loginLayout->getWidget("RegisterButton");
			registerButton->eventMouseButtonClick += MyGUI::newDelegate(this, &LoginWindowHandler::handleRegisterClick);

			Widget* exitButton = loginLayout->getWidget("ExitButton");
			exitButton->eventMouseButtonClick += MyGUI::newDelegate(this, &LoginWindowHandler::handleExitClick);

			Widget* resetPasswordButton = loginLayout->getWidget("ForgotButton");
			this->resetPasswordButtonHandler = WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(resetPasswordButton, loginLayout, resetPasswordLayout));
			
			Widget* optionsButton = loginLayout->getWidget("OptionsButton");
			optionsButton->eventMouseButtonClick += MyGUI::newDelegate(this, &LoginWindowHandler::handleOptionsButton);

			this->usernameOrEmailEditbox = static_cast<MyGUI::EditBox*>(loginLayout->getWidget("UsernameOrEmailEditbox"));
			this->usernameOrEmailEditbox->eventEditTextChange += MyGUI::newDelegate(this, &LoginWindowHandler::handleUsernameOrEmailChange);
			this->usernameOrEmailEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &LoginWindowHandler::handleAccept);

			this->passwordEditbox = static_cast<MyGUI::EditBox*>(loginLayout->getWidget("PasswordEditbox"));
			this->passwordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &LoginWindowHandler::handleAccept);

			this->errorMessageTextbox = static_cast<MyGUI::TextBox*>(loginLayout->getWidget("ErrorMessageTextbox"));

			this->rememberMeCheckboxHandler = RememberMeCheckboxHandlerPtr(new RememberMeCheckboxHandler(loginLayout->getWidget("RememberMeCheckbox")));
			this->rememberMeCheckboxHandler->setCheckboxState(false);
		}

		virtual ~LoginWindowHandler() {}

	protected:
		GuiLayoutPtr loginLayout;
		GuiLayoutPtr mainLayout; 
		GuiLayoutPtr activationLayout;
		GuiLayoutPtr registerLayout;
		GuiLayoutPtr optionsLayout;
		MyGUI::EditBox* usernameOrEmailEditbox;
		MyGUI::EditBox* passwordEditbox;
		MyGUI::TextBox* errorMessageTextbox;
		TerrorClientPtr terrorClient;
		PlayerProfilePtr playerProfile;
		ExitTriggerPtr exitTrigger;
		LoginSaverPtr loginSaver;
		WindowCallerButtonHandlerPtr resetPasswordButtonHandler;
		RememberMeCheckboxHandlerPtr rememberMeCheckboxHandler;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == LOGIN_RESPONSE) {
				LoginResponsePtr loginResponse(new LoginResponse);
				tcpPacket->buildObject(loginResponse);

				this->loginLayout->enable();
				if (loginResponse->getLoginError() == NO_LOGIN_ERROR) {
					this->playerProfile->setUsername(loginResponse->getUsername());
					this->playerProfile->setEmail(loginResponse->getEmail());
					if (this->rememberMeCheckboxHandler->getCheckboxState()) {
						this->loginSaver->login = this->usernameOrEmailEditbox->getOnlyText();
						this->loginSaver->password = this->passwordEditbox->getOnlyText();
						this->loginSaver->save();
					}
					this->loginLayout->hide();
					if (loginResponse->isActivated()) {
						this->mainLayout->enable();
					}else {
						this->activationLayout->show();
					}
				}else if (loginResponse->getLoginError() == LOGIN_FAILED) {
					this->errorMessageTextbox->setCaption("Username/email and password doesn't match!");
				}else if (loginResponse->getLoginError() == ALREADY_LOGGED) {					
					this->errorMessageTextbox->setCaption("Your account is being used in another location!");
				}else if (loginResponse->getLoginError() == LOGIN_SERVER_FAULT) {					
					this->errorMessageTextbox->setCaption("Server failed to validate your login, please try again!");
				}else {
					this->errorMessageTextbox->setCaption("Unexpected error!");
				}
			}
		}

		bool validateLogin(const string& usernameOrEmail, const string& passwordAttempt) {
			if (usernameOrEmail == "") {
				this->errorMessageTextbox->setCaption("Please enter your username or email!");
				return false;
			}

			if (passwordAttempt == "") {
				this->errorMessageTextbox->setCaption("Please enter your password!");
				return false;
			}

			if (!contains(usernameOrEmail, "@")) {
				string usernameOrEmailWithoutPermittedCharacters = usernameOrEmail;
				erase_all(usernameOrEmailWithoutPermittedCharacters, "_");
				erase_all(usernameOrEmailWithoutPermittedCharacters, ".");
				if (!all(usernameOrEmailWithoutPermittedCharacters, is_alnum())) {
					this->errorMessageTextbox->setCaption("Username/email and password doesn't match!");
					return false;
				}
			}
			return true;
		}

		void handleUsernameOrEmailChange(Widget* widget) {
			string usernameOrEmail = this->usernameOrEmailEditbox->getOnlyText();
			if (usernameOrEmail.size() > 100) {
				usernameOrEmail = usernameOrEmail.substr(0, 100);
				this->usernameOrEmailEditbox->setCaption(usernameOrEmail);
			}
		}

		void handleAccept(Widget* widget) {
			string usernameOrEmail = this->usernameOrEmailEditbox->getOnlyText();
			string password = this->passwordEditbox->getOnlyText();
			if (this->validateLogin(usernameOrEmail, password)) {
				SteamUdpAddressPtr steamUdpAddress(new SteamUdpAddress(CSteamID()));
				if (SteamUser()) {
					steamUdpAddress = SteamUdpAddressPtr(new SteamUdpAddress(SteamUser()->GetSteamID()));
				}
				LoginRequestPtr loginRequest(new LoginRequest(usernameOrEmail, password));
				this->terrorClient->sendObject(loginRequest);
				this->loginLayout->disable();
			}
		}

		void handleLoginClick(Widget* button) {
			string usernameOrEmail = this->usernameOrEmailEditbox->getOnlyText();
			string password = this->passwordEditbox->getOnlyText();
			if (this->validateLogin(usernameOrEmail, password)) {
				SteamUdpAddressPtr steamUdpAddress(new SteamUdpAddress(CSteamID()));
				if (SteamUser()) {
					steamUdpAddress = SteamUdpAddressPtr(new SteamUdpAddress(SteamUser()->GetSteamID()));
				}
				LoginRequestPtr loginRequest(new LoginRequest(usernameOrEmail, password));
				this->terrorClient->sendObject(loginRequest);
				this->loginLayout->disable();
			}
		}

		void handleRegisterClick(Widget* button) {
			this->usernameOrEmailEditbox->setCaption("");
			this->passwordEditbox->setCaption("");
			this->loginLayout->hide();
			this->registerLayout->show();
		}

		void handleExitClick(Widget* button) {
			this->exitTrigger->exit();
		}

		void handleLayoutState() {
			this->passwordEditbox->setCaption("");
			this->rememberMeCheckboxHandler->setCheckboxState(false);
			this->mainLayout->show();
			this->mainLayout->disable();
		}

		void handleOptionsButton(Widget* button) {
			this->mainLayout->hide();
			this->optionsLayout->show(this->loginLayout, false);
		}
	};

	typedef SharedPointer<LoginWindowHandler> LoginWindowHandlerPtr;
}

#endif
#ifndef TERROR_REGISTER_WINDOW_HANDLER_H
#define TERROR_REGISTER_WINDOW_HANDLER_H

#include "Config.h"

namespace terror {
	class RegisterWindowHandler : public NetworkObserver, public InputHandler {
	public:
		RegisterWindowHandler(GuiLayoutPtr registerLayout, GuiLayoutPtr loginLayout, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave)
			: NetworkObserver(false), registerLayout(registerLayout), loginLayout(loginLayout), terrorClient(terrorClient), terrorSave(terrorSave)
		{
			Widget* registerButton = registerLayout->getWidget("RegisterButton");
			registerButton->eventMouseButtonClick += MyGUI::newDelegate(this, &RegisterWindowHandler::handleRegisterClick);

			Widget* cancelButton = registerLayout->getWidget("CancelButton");
			cancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &RegisterWindowHandler::handleCancelClick);

			this->usernameEditbox = static_cast<MyGUI::EditBox*>(registerLayout->getWidget("UsernameEditbox"));
			this->usernameEditbox->eventEditTextChange += MyGUI::newDelegate(this, &RegisterWindowHandler::handleUsernameOrEmailChange);
			this->usernameEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &RegisterWindowHandler::handleAccept);

			this->emailEditbox = static_cast<MyGUI::EditBox*>(registerLayout->getWidget("EmailEditbox"));
			this->emailEditbox->eventEditTextChange += MyGUI::newDelegate(this, &RegisterWindowHandler::handleUsernameOrEmailChange);
			this->emailEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &RegisterWindowHandler::handleAccept);

			this->confirmEmailEditbox = static_cast<MyGUI::EditBox*>(registerLayout->getWidget("ConfirmEmailEditbox"));
			this->confirmEmailEditbox->eventEditTextChange += MyGUI::newDelegate(this, &RegisterWindowHandler::handleUsernameOrEmailChange);
			this->confirmEmailEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &RegisterWindowHandler::handleAccept);

			this->passwordEditbox = static_cast<MyGUI::EditBox*>(registerLayout->getWidget("PasswordEditbox"));
			this->passwordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &RegisterWindowHandler::handleAccept);

			this->confirmPasswordEditbox = static_cast<MyGUI::EditBox*>(registerLayout->getWidget("ConfirmPasswordEditbox"));
			this->confirmPasswordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &RegisterWindowHandler::handleAccept);

			this->errorMessageTextbox = static_cast<MyGUI::TextBox*>(registerLayout->getWidget("ErrorMessageTextbox"));
			this->loginMessageTextbox = static_cast<MyGUI::TextBox*>(loginLayout->getWidget("ErrorMessageTextbox"));
		}

		virtual ~RegisterWindowHandler() {}

	protected:
		GuiLayoutPtr registerLayout;
		GuiLayoutPtr loginLayout;		
		MyGUI::EditBox* usernameEditbox;
		MyGUI::EditBox* emailEditbox;
		MyGUI::EditBox* confirmEmailEditbox;
		MyGUI::EditBox* passwordEditbox;
		MyGUI::EditBox* confirmPasswordEditbox;
		MyGUI::TextBox* errorMessageTextbox;
		MyGUI::TextBox* loginMessageTextbox;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == REGISTRATION_RESPONSE) {
				RegistrationResponsePtr registrationResponse(new RegistrationResponse);
				tcpPacket->buildObject(registrationResponse);

				this->registerLayout->enable();
				if (registrationResponse->getRegistrationError() == NO_REGISTRATION_ERROR) {
					this->usernameEditbox->setCaption("");
					this->emailEditbox->setCaption("");
					this->confirmEmailEditbox->setCaption("");
					this->passwordEditbox->setCaption("");
					this->confirmPasswordEditbox->setCaption("");
					this->registerLayout->hide();
					this->loginLayout->show();
					this->loginMessageTextbox->setCaption("#00FF00User successfully registered!#FF0000");
				}else if (registrationResponse->getRegistrationError() == DUPLICATE_USERNAME) {
					this->errorMessageTextbox->setCaption("Username " + this->usernameEditbox->getOnlyText() + " is already taken!");
				}else if (registrationResponse->getRegistrationError() == DUPLICATE_EMAIL) {
					this->errorMessageTextbox->setCaption("Email " + this->emailEditbox->getOnlyText() + " is already registered!");
				}else if (registrationResponse->getRegistrationError() == REGISTRATION_SERVER_FAULT) {
					this->errorMessageTextbox->setCaption("Server failed to execute your registration, please try again!");
				}else {
					this->errorMessageTextbox->setCaption("Unexpected error!");
				}
			}
		}

		bool validateRegister() {
			string username = this->usernameEditbox->getOnlyText();
			if (username == "") {
				this->errorMessageTextbox->setCaption("Please enter a username [Aa-Zz, 0-9, _, .]!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			if (username.size() < 6) {
				this->errorMessageTextbox->setCaption("Usernames must have at least 6 characters!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			string usernameWithoutPermittedCharacters = username;
			erase_all(usernameWithoutPermittedCharacters, "_");
			erase_all(usernameWithoutPermittedCharacters, ".");
			if (!all(usernameWithoutPermittedCharacters, is_alnum())) {
				this->errorMessageTextbox->setCaption("Please enter a username [Aa-Zz, 0-9, _, .]!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			string email = this->emailEditbox->getOnlyText();
			if (email == "" || !contains(email, "@") || !contains(email, ".") || email.size() < 6) {
				this->errorMessageTextbox->setCaption("Please enter a valid email!");
				this->confirmEmailEditbox->setCaption("");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			string emailConfirmation = this->confirmEmailEditbox->getOnlyText();
			if (emailConfirmation != email) {
				this->errorMessageTextbox->setCaption("Email confirmation failed!");
				this->confirmEmailEditbox->setCaption("");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			string password = this->passwordEditbox->getOnlyText();
			if (password == "") {
				this->errorMessageTextbox->setCaption("Please define your password!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			if (password.size() < 8) {
				this->errorMessageTextbox->setCaption("Passwords must have at least 8 characters!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			string passwordConfirmation = this->confirmPasswordEditbox->getOnlyText();
			if (passwordConfirmation != password) {
				this->errorMessageTextbox->setCaption("Password confirmation failed!");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				return false;
			}

			return true;
		}

		void handleUsernameOrEmailChange(Widget* widget) {
			MyGUI::EditBox* editbox = static_cast<MyGUI::EditBox*>(widget);
			string usernameOrEmail = editbox->getOnlyText();
			if (usernameOrEmail.size() > 100) {
				usernameOrEmail = usernameOrEmail.substr(0, 100);
				editbox->setCaption(usernameOrEmail);
			}
		}

		void handleAccept(Widget* widget) {
			if (this->validateRegister()) {
				string username = this->usernameEditbox->getOnlyText();
				string email = this->emailEditbox->getOnlyText();
				string password = this->passwordEditbox->getOnlyText();
				RegistrationRequestPtr registrationRequest(new RegistrationRequest(username, email, password));
				this->terrorClient->sendObject(registrationRequest);
				this->registerLayout->disable();
			}
		}

		void handleRegisterClick(Widget* button) {
			if (this->validateRegister()) {
				string username = this->usernameEditbox->getOnlyText();
				string email = this->emailEditbox->getOnlyText();
				string password = this->passwordEditbox->getOnlyText();
				RegistrationRequestPtr registrationRequest(new RegistrationRequest(username, email, password));
				this->terrorClient->sendObject(registrationRequest);
				this->registerLayout->disable();
			}
		}

		void handleCancelClick(Widget* button) {
			this->usernameEditbox->setCaption("");
			this->emailEditbox->setCaption("");
			this->confirmEmailEditbox->setCaption("");
			this->passwordEditbox->setCaption("");
			this->confirmPasswordEditbox->setCaption("");
			this->registerLayout->hide();
			this->loginLayout->show();
		}	

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->registerLayout->isVisible()) {
				this->usernameEditbox->setCaption("");
				this->emailEditbox->setCaption("");
				this->confirmEmailEditbox->setCaption("");
				this->passwordEditbox->setCaption("");
				this->confirmPasswordEditbox->setCaption("");
				this->registerLayout->hide();
				this->loginLayout->show();
			}
		}
	};

	typedef SharedPointer<RegisterWindowHandler> RegisterWindowHandlerPtr;
}

#endif
#ifndef TERROR_PROFILE_WINDOW_HANDLER_H
#define TERROR_PROFILE_WINDOW_HANDLER_H

#include "PlayerProfile/PlayerProfile.h"

namespace terror {
	class ProfileWindowHandler : public NetworkObserver, public InputHandler {
	public:
		ProfileWindowHandler(GuiLayoutPtr profileLayout, PlayerProfilePtr playerProfile, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave) 
			: NetworkObserver(false), profileLayout(profileLayout), playerProfile(playerProfile), terrorClient(terrorClient), terrorSave(terrorSave)
		{
			profileLayout->connectShowEvent(Bind(&ProfileWindowHandler::updateProfile, this));

			this->usernameTextbox = static_cast<MyGUI::TextBox*>(profileLayout->getWidget("UsernameTextbox"));
			this->emailTextbox = static_cast<MyGUI::TextBox*>(profileLayout->getWidget("EmailTextbox"));
			this->errorMessageTextbox = static_cast<MyGUI::TextBox*>(profileLayout->getWidget("ErrorMessageTextbox"));

			this->newEmailEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewEmailEditbox"));
			this->newEmailConfirmationEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewEmailConfirmationEditbox"));
			this->newUsernameEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewUsernameEditbox"));
			this->newUsernameConfirmationEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewUsernameConfirmationEditbox"));
			this->newPasswordEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewPasswordEditbox"));
			this->newPasswordConfirmationEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("NewPasswordConfirmationEditbox"));
			this->passwordEditbox = static_cast<MyGUI::EditBox*>(profileLayout->getWidget("PasswordEditbox"));

			Widget* cancelButton = profileLayout->getWidget("CancelButton");
			cancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ProfileWindowHandler::handleCancelButtonClick);

			Widget* okButton = profileLayout->getWidget("OkButton");
			okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ProfileWindowHandler::handleOkButtonClick);

			this->passwordEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ProfileWindowHandler::handlePasswordAcceptEvent);
		}

		virtual ~ProfileWindowHandler() {}

	protected:
		GuiLayoutPtr profileLayout;
		PlayerProfilePtr playerProfile;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		MyGUI::TextBox* usernameTextbox;
		MyGUI::TextBox* emailTextbox;
		MyGUI::TextBox* errorMessageTextbox;
		MyGUI::EditBox* newEmailEditbox;
		MyGUI::EditBox* newEmailConfirmationEditbox;
		MyGUI::EditBox* newUsernameEditbox;
		MyGUI::EditBox* newUsernameConfirmationEditbox;
		MyGUI::EditBox* newPasswordEditbox;
		MyGUI::EditBox* newPasswordConfirmationEditbox;
		MyGUI::EditBox* passwordEditbox;

	private:
		void handlePasswordAcceptEvent(Widget* widget) {
			this->handleOkButtonClick(0);
		}

		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == ACCOUNT_UPDATE_RESPONSE) {
				AccountUpdateResponsePtr accountUpdateResponse(new AccountUpdateResponse);
				tcpPacket->buildObject(accountUpdateResponse);

				this->profileLayout->enable();
				AccountUpdateError error = accountUpdateResponse->getAccountUpdateError();
				if (error == NO_ACCOUNT_UPDATE_ERROR) {
					string newEmail = this->newEmailEditbox->getOnlyText();
					if (newEmail != "") {
						this->playerProfile->setEmail(newEmail);
						this->emailTextbox->setCaption(newEmail);
					}
					string newUsername = this->newUsernameEditbox->getOnlyText();
					if (newUsername != "") {
						this->playerProfile->setUsername(newUsername);
						this->usernameTextbox->setCaption(newUsername);
					}
					this->profileLayout->hide();
				}else if (error == ACCOUNT_UPDATE_FAILED) {
					this->errorMessageTextbox->setCaption("Wrong password!");
				}else if (error == ACCOUNT_UPDATE_DUPLICATE_EMAIL) {				
					this->errorMessageTextbox->setCaption("Email is already registered!");
				}else if (error == ACCOUNT_UPDATE_DUPLICATE_USERNAME) {					
					this->errorMessageTextbox->setCaption("Username is already registered!");
				}else if (error == ACCOUNT_UPDATE_SERVER_FAULT) {					
					this->errorMessageTextbox->setCaption("Server failed to validate your account, please try again!");
				}else {
					this->errorMessageTextbox->setCaption("Unexpected error!");
				}
			}
		}

		void updateProfile() {
			this->usernameTextbox->setCaption(this->playerProfile->getUsername());
			this->emailTextbox->setCaption(this->playerProfile->getEmail());
			this->newEmailEditbox->setCaption("");
			this->newEmailConfirmationEditbox->setCaption("");
			this->newUsernameEditbox->setCaption("");
			this->newUsernameConfirmationEditbox->setCaption("");
			this->newPasswordEditbox->setCaption("");
			this->newPasswordConfirmationEditbox->setCaption("");
			this->passwordEditbox->setCaption("");
		}

		void handleCancelButtonClick(Widget* button) {
			this->profileLayout->hide();
		}

		void handleOkButtonClick(Widget* button) {
			string newEmail = this->newEmailEditbox->getOnlyText();
			string newUsername = this->newUsernameEditbox->getOnlyText();
			string newPassword = this->newPasswordEditbox->getOnlyText();

			if (newEmail == "" && newPassword == "" && newUsername == "") {
				this->profileLayout->hide();
				return;
			}
			if (newEmail != "") {
				EmailValidatorPtr emailValidator(new EmailValidator);
				if (!emailValidator->validate(newEmail)) {
					this->errorMessageTextbox->setCaption("Please enter a valid email!");
					return;
				}
				string newEmailConfirmation = this->newEmailConfirmationEditbox->getOnlyText();
				if (newEmail != newEmailConfirmation) {
					this->errorMessageTextbox->setCaption("Email confirmation failed!");
					return;
				}
			}
			if (newUsername != "") {
				UsernameValidatorPtr usernameValidator(new UsernameValidator);
				if (!usernameValidator->validate(newUsername)) {
					this->errorMessageTextbox->setCaption("Please enter a valid username!");
					return;
				}
				if (newUsername != this->newUsernameConfirmationEditbox->getOnlyText()) {
					this->errorMessageTextbox->setCaption("Username confirmation failed!");
					return;
				}
			}
			if (newPassword != "") {
				if (newPassword.size() < 8) {
					this->errorMessageTextbox->setCaption("Passwords must have at least 8 characters!");
					return;
				}
				string newPasswordConfirmation = this->newPasswordConfirmationEditbox->getOnlyText();
				if (newPassword != newPasswordConfirmation) {
					this->errorMessageTextbox->setCaption("Password confirmation failed!");
					return;
				}
			}

			string password = this->passwordEditbox->getOnlyText();
			if (password == "") {
				this->errorMessageTextbox->setCaption("Please enter your password!");
				return;
			}

			this->errorMessageTextbox->setCaption("");
			AccountUpdateRequestPtr accountUpdateRequest(new AccountUpdateRequest(this->playerProfile->getUsername(), password, newUsername, newEmail, newPassword));
			this->terrorClient->sendObject(accountUpdateRequest);
			this->profileLayout->disable();
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->profileLayout->isVisible()) {
				this->profileLayout->hide();
			}
		}
	};

	typedef SharedPointer<ProfileWindowHandler> ProfileWindowHandlerPtr;
}

#endif
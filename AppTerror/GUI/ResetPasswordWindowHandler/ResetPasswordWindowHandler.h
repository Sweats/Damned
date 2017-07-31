#ifndef TERROR_RESET_PASSWORD_WINDOW_HANDLER_H
#define TERROR_RESET_PASSWORD_WINDOW_HANDLER_H

#include "GUI/WarningMessageHandler/WarningMessageHandler.h"

namespace terror {
	class ResetPasswordWindowHandler : public NetworkObserver, public InputHandler {
	public:
		ResetPasswordWindowHandler(GuiLayoutPtr resetPasswordLayout, TerrorClientPtr terrorClient, WarningMessageHandlerPtr warningMessageHandler, TerrorSavePtr terrorSave)
			: NetworkObserver(false), resetPasswordLayout(resetPasswordLayout), terrorClient(terrorClient), warningMessageHandler(warningMessageHandler), terrorSave(terrorSave)
		{
			Widget* resetPasswordButton = resetPasswordLayout->getWidget("ResetPasswordButton");
			resetPasswordButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ResetPasswordWindowHandler::handleResetPasswordButtonClick);

			Widget* backButton = resetPasswordLayout->getWidget("BackButton");
			this->backButtonHandler = BackButtonHandlerPtr(new BackButtonHandler(backButton, resetPasswordLayout));

			this->emailEditbox = static_cast<MyGUI::EditBox*>(resetPasswordLayout->getWidget("EmailEditbox"));
			this->emailEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ResetPasswordWindowHandler::handleAccept);

			this->errorMessageTextbox = static_cast<MyGUI::TextBox*>(resetPasswordLayout->getWidget("ErrorMessageTextbox"));
		}

		virtual ~ResetPasswordWindowHandler() {}

	protected:
		GuiLayoutPtr resetPasswordLayout;
		TerrorClientPtr terrorClient;
		WarningMessageHandlerPtr warningMessageHandler;		
		TerrorSavePtr terrorSave;
		BackButtonHandlerPtr backButtonHandler;
		MyGUI::EditBox* emailEditbox;
		MyGUI::TextBox* errorMessageTextbox;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == RESET_PASSWORD_RESPONSE) {
				ResetPasswordResponsePtr resetPasswordResponse(new ResetPasswordResponse);
				tcpPacket->buildObject(resetPasswordResponse);

				this->resetPasswordLayout->enable();
				if (resetPasswordResponse->getResetPasswordError() == NO_RESET_PASSWORD_ERROR) {
					GuiLayoutPtr parent = this->resetPasswordLayout->getParent();
					this->resetPasswordLayout->hide();
					this->warningMessageHandler->showWarningMessage("Your password was reset.\nCheck your email to retrieve it.", parent);
				}else if (resetPasswordResponse->getResetPasswordError() == RESET_PASSWORD_FAILED) {
					this->errorMessageTextbox->setCaption("There are no accounts using this email!");
				}else if (resetPasswordResponse->getResetPasswordError() == RESET_PASSWORD_SERVER_FAULT) {					
					this->errorMessageTextbox->setCaption("Server failed to reset your password, please try again!");
				}else {
					this->errorMessageTextbox->setCaption("Unexpected error!");
				}
			}
		}

		void handleResetPasswordButtonClick(Widget* resetPasswordButton) {
			string email = this->emailEditbox->getOnlyText();
			if (email == "" || !contains(email, "@") || !contains(email, ".") || email.size() < 6) {
				this->errorMessageTextbox->setCaption("Please enter a valid email!");
				return;
			}

			ResetPasswordRequestPtr resetPasswordRequest(new ResetPasswordRequest(email));
			this->terrorClient->sendObject(resetPasswordRequest);
			this->resetPasswordLayout->disable();
		}

		void handleAccept(Widget* widget) {
			string email = this->emailEditbox->getOnlyText();
			if (email == "" || !contains(email, "@") || !contains(email, ".") || email.size() < 6) {
				this->errorMessageTextbox->setCaption("Please enter a valid email!");
				return;
			}

			ResetPasswordRequestPtr resetPasswordRequest(new ResetPasswordRequest(email));
			this->terrorClient->sendObject(resetPasswordRequest);
			this->resetPasswordLayout->disable();
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->resetPasswordLayout->isVisible()) {
				this->resetPasswordLayout->hide();
			}
		}
	};

	typedef SharedPointer<ResetPasswordWindowHandler> ResetPasswordWindowHandlerPtr;
}

#endif
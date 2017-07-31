#ifndef TERROR_ACTIVATION_WINDOW_HANDLER_H
#define TERROR_ACTIVATION_WINDOW_HANDLER_H

#include "ExitTrigger/ExitTrigger.h"
#include "PlayerProfile/PlayerProfile.h"
#include "GUI/TitleWindowHandler/LogoffButtonHandler/LogoffButtonHandler.h"

namespace terror {
	class ActivationWindowHandler : public NetworkObserver {
	public:
		ActivationWindowHandler(GuiLayoutPtr activationLayout, GuiLayoutPtr mainLayout, GuiLayoutPtr loginLayout, TerrorClientPtr terrorClient, 
			PlayerProfilePtr playerProfile, ExitTriggerPtr exitTrigger, LoginSaverPtr loginSaver)
			: NetworkObserver(false), activationLayout(activationLayout), mainLayout(mainLayout), terrorClient(terrorClient), playerProfile(playerProfile), exitTrigger(exitTrigger) 
		{
			Widget* activateButton = activationLayout->getWidget("ActivateButton");
			activateButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ActivationWindowHandler::handleActivateClick);

			Widget* exitButton = activationLayout->getWidget("ExitButton");
			exitButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ActivationWindowHandler::handleExitClick);

			this->serial1Editbox = static_cast<MyGUI::EditBox*>(activationLayout->getWidget("Serial1Editbox"));
			this->serial1Editbox->eventEditTextChange += MyGUI::newDelegate(this, &ActivationWindowHandler::handleSerialChange);
			this->serial1Editbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ActivationWindowHandler::handleAccept);

			this->serial2Editbox = static_cast<MyGUI::EditBox*>(activationLayout->getWidget("Serial2Editbox"));
			this->serial2Editbox->eventEditTextChange += MyGUI::newDelegate(this, &ActivationWindowHandler::handleSerialChange);
			this->serial2Editbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ActivationWindowHandler::handleAccept);

			this->serial3Editbox = static_cast<MyGUI::EditBox*>(activationLayout->getWidget("Serial3Editbox"));
			this->serial3Editbox->eventEditTextChange += MyGUI::newDelegate(this, &ActivationWindowHandler::handleSerialChange);
			this->serial3Editbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ActivationWindowHandler::handleAccept);

			this->serial4Editbox = static_cast<MyGUI::EditBox*>(activationLayout->getWidget("Serial4Editbox"));
			this->serial4Editbox->eventEditTextChange += MyGUI::newDelegate(this, &ActivationWindowHandler::handleSerialChange);
			this->serial4Editbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ActivationWindowHandler::handleAccept);

			this->serial5Editbox = static_cast<MyGUI::EditBox*>(activationLayout->getWidget("Serial5Editbox"));
			this->serial5Editbox->eventEditTextChange += MyGUI::newDelegate(this, &ActivationWindowHandler::handleSerialChange);
			this->serial5Editbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ActivationWindowHandler::handleAccept);

			this->errorMessageTextbox = static_cast<MyGUI::TextBox*>(activationLayout->getWidget("ErrorMessageTextbox"));

			this->logoffButtonHandler = LogoffButtonHandlerPtr(new LogoffButtonHandler(activationLayout, loginLayout, loginSaver, playerProfile, terrorClient));
		}

		virtual ~ActivationWindowHandler() {}

	protected:
		GuiLayoutPtr mainLayout; 
		GuiLayoutPtr activationLayout;
		MyGUI::EditBox* serial1Editbox;
		MyGUI::EditBox* serial2Editbox;
		MyGUI::EditBox* serial3Editbox;
		MyGUI::EditBox* serial4Editbox;
		MyGUI::EditBox* serial5Editbox;
		MyGUI::TextBox* errorMessageTextbox;
		LogoffButtonHandlerPtr logoffButtonHandler;
		TerrorClientPtr terrorClient;
		PlayerProfilePtr playerProfile;
		ExitTriggerPtr exitTrigger;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == ACTIVATION_RESPONSE) {
				ActivationResponsePtr activationResponse(new ActivationResponse);
				tcpPacket->buildObject(activationResponse);

				this->activationLayout->enable();
				if (activationResponse->getActivationError() == NO_ACTIVATION_ERROR) {
					this->activationLayout->hide();
					this->mainLayout->enable();
				}else if (activationResponse->getActivationError() == INVALID_SERIAL) {
					this->errorMessageTextbox->setCaption("Invalid serial number!");
				}else if (activationResponse->getActivationError() == ALREADY_ACTIVATED) {					
					this->errorMessageTextbox->setCaption("This serial is already activated!");
				}else if (activationResponse->getActivationError() == ACTIVATION_SERVER_FAULT) {					
					this->errorMessageTextbox->setCaption("Server failed to execute your activation, please try again!");
				}else {
					this->errorMessageTextbox->setCaption("Unexpected error!");
				}
			}
		}

		bool validateSerial() {
			if (this->serial1Editbox->getOnlyText() == "" || this->serial2Editbox->getOnlyText() == "" || this->serial3Editbox->getOnlyText() == "" ||
				this->serial4Editbox->getOnlyText() == "" || this->serial5Editbox->getOnlyText() == "") 
			{
				return false;
			}

			int serialPart1 = -1;
			int serialPart2 = -1;
			int serialPart3 = -1;
			int serialPart4 = -1;
			int serialPart5 = -1; 
			try {
				serialPart1 = lexical_cast<int>(this->serial1Editbox->getOnlyText());
				serialPart2 = lexical_cast<int>(this->serial2Editbox->getOnlyText());
				serialPart3 = lexical_cast<int>(this->serial3Editbox->getOnlyText());
				serialPart4 = lexical_cast<int>(this->serial4Editbox->getOnlyText());
				serialPart5 = lexical_cast<int>(this->serial5Editbox->getOnlyText());
			}catch (boost::bad_lexical_cast&) {}

			if (serialPart1 < 10000 || serialPart1 > 99999 || serialPart2 < 10000 || serialPart2 > 99999 ||
				serialPart3 < 10000 || serialPart3 > 99999 || serialPart4 < 10000 || serialPart4 > 99999 ||
				serialPart5 < 10000 || serialPart5 > 99999) 
			{
				this->errorMessageTextbox->setCaption("Invalid serial number!");
				return false;
			}
			return true;
		}

		void handleSerialChange(Widget* widget) {
			MyGUI::EditBox* editbox = static_cast<MyGUI::EditBox*>(widget);
			string serialPart = editbox->getOnlyText();
			trim_if(serialPart, !is_digit());
			if (serialPart.size() > 5) {
				serialPart = serialPart.substr(0, 5);
			}
			editbox->setCaption(serialPart);
		}

		void handleAccept(Widget* widget) {
			if (this->validateSerial()) {
				string username = this->playerProfile->getUsername();
				string serial = this->serial1Editbox->getOnlyText() + string("-") + this->serial2Editbox->getOnlyText() + string("-") + 
					this->serial3Editbox->getOnlyText() + string("-") + this->serial4Editbox->getOnlyText() + ("-") + this->serial5Editbox->getOnlyText();
				ActivationRequestPtr activationRequest(new ActivationRequest(serial));
				this->terrorClient->sendObject(activationRequest);
				this->activationLayout->disable();
			}
		}

		void handleActivateClick(Widget* button) {
			if (this->validateSerial()) {
				string username = this->playerProfile->getUsername();
				string serial = this->serial1Editbox->getOnlyText() + string("-") + this->serial2Editbox->getOnlyText() + string("-") + 
					this->serial3Editbox->getOnlyText() + string("-") + this->serial4Editbox->getOnlyText() + ("-") + this->serial5Editbox->getOnlyText();
				ActivationRequestPtr activationRequest(new ActivationRequest(serial));
				this->terrorClient->sendObject(activationRequest);
				this->activationLayout->disable();
			}
		}

		void handleExitClick(Widget* button) {
			this->exitTrigger->exit();
		}
	};

	typedef SharedPointer<ActivationWindowHandler> ActivationWindowHandlerPtr;
}

#endif
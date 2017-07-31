#ifndef TERROR_WARNING_MESSAGE_HANDLER_H
#define TERROR_WARNING_MESSAGE_HANDLER_H

#include "Config.h"

namespace terror {
	class WarningMessageHandler {
	public:
		WarningMessageHandler(GuiLayoutPtr warningMessageLayout) 
			: warningMessageLayout(warningMessageLayout) 
		{
			Widget* okButton = warningMessageLayout->getWidget("OkButton");
			okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &WarningMessageHandler::handleOkButton);

			this->warningMessageTextbox = static_cast<MyGUI::TextBox*>(warningMessageLayout->getWidget("WarningMessageTextbox"));
		}

		void showWarningMessage(const std::string& warningMessage, GuiLayoutPtr parent) {		
			this->parent = parent;
			if (this->parent) {
				this->parent->disable();
			}
			this->warningMessageLayout->show();
			this->warningMessageTextbox->setCaption(warningMessage);
		}

		virtual ~WarningMessageHandler() {}

	protected:
		GuiLayoutPtr warningMessageLayout;	
		MyGUI::TextBox* warningMessageTextbox;
		GuiLayoutPtr parent;

	private:
		void handleOkButton(Widget* okButton) {
			this->warningMessageLayout->hide();
			if (this->parent) {
				this->parent->enable();
			}
		}
	};

	typedef SharedPointer<WarningMessageHandler> WarningMessageHandlerPtr;
}

#endif
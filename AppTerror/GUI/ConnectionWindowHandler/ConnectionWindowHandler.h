#ifndef TERROR_CONNECTION_WINDOW_HANDLER_H
#define TERROR_CONNECTION_WINDOW_HANDLER_H

#include "Config.h"

namespace terror {
	class FixedPortOptionsHandler : public CheckboxHandler {
	public:
		FixedPortOptionsHandler(Widget* useFixedPortCheckbox, MyGUI::EditBox* fixedPortEditbox, Widget* saveButton, TerrorSavePtr terrorSave) 
			: CheckboxHandler(useFixedPortCheckbox), fixedPortEditbox(fixedPortEditbox), terrorSave(terrorSave) 
		{
			saveButton->eventMouseButtonClick += MyGUI::newDelegate(this, &FixedPortOptionsHandler::handleSaveButtonClick);
			this->fixedPortEditbox->eventEditTextChange += MyGUI::newDelegate(this, &FixedPortOptionsHandler::handleEditboxChange);			
		}

		void resetScreen() {
			if (this->terrorSave->useFixedPort) {
				this->checkbox->setStateSelected(true);
				this->fixedPortEditbox->setCaption(lexical_cast<string>(this->terrorSave->fixedPort));
				this->fixedPortEditbox->setEnabled(true);
			}else {
				this->checkbox->setStateSelected(false);
				this->fixedPortEditbox->setCaption("");
				this->fixedPortEditbox->setEnabled(false);
			}
		}

		virtual ~FixedPortOptionsHandler() {}

	protected:
		MyGUI::EditBox* fixedPortEditbox;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->useFixedPort = state;
			this->terrorSave->save();
			if (this->terrorSave->useFixedPort) {
				this->fixedPortEditbox->setCaption(lexical_cast<string>(this->terrorSave->fixedPort));
				this->fixedPortEditbox->setEnabled(true);
			}else {
				this->fixedPortEditbox->setCaption("");
				this->fixedPortEditbox->setEnabled(false);
			}
		}

		virtual void handleSaveButtonClick(Widget* button) {
			string newText = this->fixedPortEditbox->getOnlyText();
			trim_if(newText, !is_digit());
			if (newText == "") {
				newText = "1024";
			}
			int newValue = clamp(lexical_cast<int>(newText), 1024, 65535);
			this->fixedPortEditbox->setCaption(lexical_cast<string>(newValue));
			this->terrorSave->fixedPort = newValue;
			this->terrorSave->save();
		}

		void handleEditboxChange(MyGUI::EditBox* widget) {
			string newText = widget->getOnlyText();
			trim_if(newText, !is_digit());
		}
	};
	typedef SharedPointer<FixedPortOptionsHandler> FixedPortOptionsHandlerPtr;

	class ConnectionWindowHandler : public InputHandler {
	public:
		ConnectionWindowHandler(GuiLayoutPtr connectionLayout, TerrorSavePtr terrorSave) 
			: connectionLayout(connectionLayout), terrorSave(terrorSave)
		{
			Widget* useFixedPortCheckbox = connectionLayout->getWidget("UseFixedPortCheckbox");
			MyGUI::EditBox* fixedPortEditbox = static_cast<MyGUI::EditBox*>(connectionLayout->getWidget("FixedPortEditbox"));
			Widget* saveButton = connectionLayout->getWidget("FixedPortSaveButton");
			this->fixedPortOptionsHandler = 
				FixedPortOptionsHandlerPtr(new FixedPortOptionsHandler(useFixedPortCheckbox, fixedPortEditbox, saveButton, terrorSave));
			connectionLayout->connectShowEvent(Bind(&FixedPortOptionsHandler::resetScreen, this->fixedPortOptionsHandler));
			
			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(connectionLayout->getWidget("BackButton"), connectionLayout));
		}

		virtual ~ConnectionWindowHandler() {}

	protected:
		GuiLayoutPtr connectionLayout;
		TerrorSavePtr terrorSave;
		FixedPortOptionsHandlerPtr fixedPortOptionsHandler;
		BackButtonHandlerPtr backButtonHandler;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->connectionLayout->isVisible()) {
				this->connectionLayout->hide();
			}
		}
	};

	typedef SharedPointer<ConnectionWindowHandler> ConnectionWindowHandlerPtr;
}

#endif
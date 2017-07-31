#ifndef TERROR_START_BUTTON_HANDLER_H
#define TERROR_START_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class StartButtonHandler : public ControlButtonHandler {
	public:
		StartButtonHandler(Widget* startButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(startButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->start;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->start == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->start));
		}

		virtual ~StartButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->start = keyCode;
		}
	};

	typedef SharedPointer<StartButtonHandler> StartButtonHandlerPtr;
}

#endif
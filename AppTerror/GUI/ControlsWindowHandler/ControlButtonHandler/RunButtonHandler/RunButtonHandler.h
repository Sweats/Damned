#ifndef TERROR_RUN_BUTTON_HANDLER_H
#define TERROR_RUN_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class RunButtonHandler : public ControlButtonHandler {
	public:
		RunButtonHandler(Widget* runButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(runButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->run;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->run == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->run));
		}

		virtual ~RunButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->run = keyCode;
		}
	};

	typedef SharedPointer<RunButtonHandler> RunButtonHandlerPtr;
}

#endif
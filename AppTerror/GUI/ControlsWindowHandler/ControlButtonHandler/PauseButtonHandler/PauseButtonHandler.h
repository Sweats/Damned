#ifndef TERROR_PAUSE_BUTTON_HANDLER_H
#define TERROR_PAUSE_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class PauseButtonHandler : public ControlButtonHandler {
	public:
		PauseButtonHandler(Widget* pauseButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(pauseButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->pause;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->pause == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->pause));
		}

		virtual ~PauseButtonHandler() {}

	private:
		virtual bool checkKeyValidity(const ButtonCode& buttonCode){ 
			return true;
		}

		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->pause = keyCode;
		}
	};

	typedef SharedPointer<PauseButtonHandler> PauseButtonHandlerPtr;
}

#endif
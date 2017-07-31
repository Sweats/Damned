#ifndef TERROR_SPECTATE_NEXT_BUTTON_HANDLER_H
#define TERROR_SPECTATE_NEXT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class SpectateNextButtonHandler : public ControlButtonHandler {
	public:
		SpectateNextButtonHandler(Widget* spectateNextButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(spectateNextButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->spectateNext;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->spectateNext == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->spectateNext));
		}

		virtual ~SpectateNextButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->spectateNext = keyCode;
		}
	};

	typedef SharedPointer<SpectateNextButtonHandler> SpectateNextButtonHandlerPtr;
}

#endif
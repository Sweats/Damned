#ifndef TERROR_CROUCH_BUTTON_HANDLER_H
#define TERROR_CROUCH_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class CrouchButtonHandler : public ControlButtonHandler {
	public:
		CrouchButtonHandler(Widget* crouchButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(crouchButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->crouch;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->crouch == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->crouch));
		}

		virtual ~CrouchButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->crouch = keyCode;
		}
	};

	typedef SharedPointer<CrouchButtonHandler> CrouchButtonHandlerPtr;
}

#endif
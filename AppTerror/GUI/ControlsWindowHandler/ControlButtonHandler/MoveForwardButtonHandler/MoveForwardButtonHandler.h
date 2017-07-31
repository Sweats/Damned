#ifndef TERROR_MOVE_FORWARD_BUTTON_HANDLER_H
#define TERROR_MOVE_FORWARD_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class MoveForwardButtonHandler : public ControlButtonHandler {
	public:
		MoveForwardButtonHandler(Widget* moveForwardButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(moveForwardButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->moveForward;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->moveForward == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->moveForward));
		}

		virtual ~MoveForwardButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->moveForward = keyCode;
		}
	};

	typedef SharedPointer<MoveForwardButtonHandler> MoveForwardButtonHandlerPtr;
}

#endif
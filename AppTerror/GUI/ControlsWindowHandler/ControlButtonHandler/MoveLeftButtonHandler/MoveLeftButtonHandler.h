#ifndef TERROR_MOVE_LEFT_BUTTON_HANDLER_H
#define TERROR_MOVE_LEFT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class MoveLeftButtonHandler : public ControlButtonHandler {
	public:
		MoveLeftButtonHandler(Widget* moveLeftButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(moveLeftButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->moveLeft;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->moveLeft == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->moveLeft));
		}

		virtual ~MoveLeftButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->moveLeft = keyCode;
		}
	};

	typedef SharedPointer<MoveLeftButtonHandler> MoveLeftButtonHandlerPtr;
}

#endif
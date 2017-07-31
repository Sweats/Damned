#ifndef TERROR_MOVE_RIGHT_BUTTON_HANDLER_H
#define TERROR_MOVE_RIGHT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class MoveRightButtonHandler : public ControlButtonHandler {
	public:
		MoveRightButtonHandler(Widget* moveRightButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(moveRightButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->moveRight;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->moveRight == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->moveRight));
		}

		virtual ~MoveRightButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->moveRight = keyCode;
		}
	};

	typedef SharedPointer<MoveRightButtonHandler> MoveRightButtonHandlerPtr;
}

#endif
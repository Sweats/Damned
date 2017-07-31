#ifndef TERROR_MOVE_BACK_BUTTON_HANDLER_H
#define TERROR_MOVE_BACK_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class MoveBackButtonHandler : public ControlButtonHandler {
	public:
		MoveBackButtonHandler(Widget* moveBackButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(moveBackButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->moveBack;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->moveBack == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->moveBack));
		}

		virtual ~MoveBackButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->moveBack = keyCode;
		}
	};

	typedef SharedPointer<MoveBackButtonHandler> MoveBackButtonHandlerPtr;
}

#endif
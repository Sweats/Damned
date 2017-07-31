#ifndef TERROR_ATTACK_BUTTON_HANDLER_H
#define TERROR_ATTACK_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class AttackButtonHandler : public ControlButtonHandler {
	public:
		AttackButtonHandler(Widget* attackButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(attackButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->attack;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->attack == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->attack));
		}

		virtual ~AttackButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->attack = keyCode;
		}
	};

	typedef SharedPointer<AttackButtonHandler> AttackButtonHandlerPtr;
}

#endif
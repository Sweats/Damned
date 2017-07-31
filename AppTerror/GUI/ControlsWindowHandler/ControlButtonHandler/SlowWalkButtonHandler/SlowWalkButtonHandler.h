#ifndef TERROR_SLOW_WALK_BUTTON_HANDLER_H
#define TERROR_SLOW_WALK_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class SlowWalkButtonHandler : public ControlButtonHandler {
	public:
		SlowWalkButtonHandler(Widget* slowWalkButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(slowWalkButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->slowWalk;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->slowWalk == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->slowWalk));
		}

		virtual ~SlowWalkButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->slowWalk = keyCode;
		}
	};

	typedef SharedPointer<SlowWalkButtonHandler> SlowWalkButtonHandlerPtr;
}

#endif
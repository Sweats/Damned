#ifndef TERROR_GRAB_BUTTON_HANDLER_H
#define TERROR_GRAB_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class GrabButtonHandler : public ControlButtonHandler {
	public:
		GrabButtonHandler(Widget* grabButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(grabButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->grab;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->grab == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->grab));
		}

		virtual ~GrabButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->grab = keyCode;
		}
	};

	typedef SharedPointer<GrabButtonHandler> GrabButtonHandlerPtr;
}

#endif
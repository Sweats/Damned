#ifndef TERROR_TRASNFORM_BUTTON_HANDLER_H
#define TERROR_TRASNFORM_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class TransformButtonHandler : public ControlButtonHandler {
	public:
		TransformButtonHandler(Widget* transformButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(transformButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->transform;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->transform == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->transform));
		}

		virtual ~TransformButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->transform = keyCode;
		}
	};

	typedef SharedPointer<TransformButtonHandler> TransformButtonHandlerPtr;
}

#endif
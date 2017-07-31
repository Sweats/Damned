#ifndef TERROR_SWITCH_FLASHLIGHT_BUTTON_HANDLER_H
#define TERROR_SWITCH_FLASHLIGHT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class SwitchFlashlightButtonHandler : public ControlButtonHandler {
	public:
		SwitchFlashlightButtonHandler(Widget* switchFlashlightButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(switchFlashlightButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->switchFlashlight;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->switchFlashlight == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->switchFlashlight));
		}

		virtual ~SwitchFlashlightButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->switchFlashlight = keyCode;
		}
	};

	typedef SharedPointer<SwitchFlashlightButtonHandler> SwitchFlashlightButtonHandlerPtr;
}

#endif
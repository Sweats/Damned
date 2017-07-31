#ifndef TERROR_TELEPORT_BUTTON_HANDLER_H
#define TERROR_TELEPORT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class TeleportButtonHandler : public ControlButtonHandler {
	public:
		TeleportButtonHandler(Widget* teleportButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(teleportButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->teleport;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->teleport == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->teleport));
		}

		virtual ~TeleportButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->teleport = keyCode;
		}
	};

	typedef SharedPointer<TeleportButtonHandler> TeleportButtonHandlerPtr;
}

#endif
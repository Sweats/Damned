#ifndef TERROR_GUI_ACCEPT_BUTTON_HANDLER_H
#define TERROR_GUI_ACCEPT_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class GuiAcceptButtonHandler : public ControlButtonHandler {
	public:
		GuiAcceptButtonHandler(Widget* guiAcceptButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator, GuiSystemPtr guiSystem)
			: ControlButtonHandler(guiAcceptButton, controlsLayout, terrorSave, buttonTranslator), guiSystem(guiSystem)
		{
			this->guiSystem->setAcceptButton(this->terrorSave->controls->guiAccept);
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->guiAccept;
		}

		void updateButtonCaption(){
			if (this->getKeyCode() == BC_UNASSIGNED){
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			}else {
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->getKeyCode()));
			}
		}

		virtual ~GuiAcceptButtonHandler() {}

	protected:
		GuiSystemPtr guiSystem;

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->guiSystem->setAcceptButton(keyCode);
			this->terrorSave->controls->guiAccept = keyCode;
		}
	};

	typedef SharedPointer<GuiAcceptButtonHandler> GuiAcceptButtonHandlerPtr;
}

#endif
#ifndef TERROR_GUI_RETURN_BUTTON_HANDLER_H
#define TERROR_GUI_RETURN_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class GuiReturnButtonHandler : public ControlButtonHandler {
	public:
		GuiReturnButtonHandler(Widget* guiReturnButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(guiReturnButton, controlsLayout, terrorSave, buttonTranslator)
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->guiReturn;
		}

		void updateButtonCaption(){
			if (this->getKeyCode() == BC_UNASSIGNED){
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			}else {
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->getKeyCode()));
			}
		}

		virtual ~GuiReturnButtonHandler() {}

	protected:
		GuiSystemPtr guiSystem;

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->guiReturn = keyCode;
		}
	};

	typedef SharedPointer<GuiReturnButtonHandler> GuiReturnButtonHandlerPtr;
}

#endif
#ifndef TERROR_CONTROL_BUTTON_HANDLER_H
#define TERROR_CONTROL_BUTTON_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class ControlButtonHandler : public ButtonHandler {
	public:
		ControlButtonHandler(Widget* controlButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ButtonHandler(controlButton), controlButton(controlButton), controlsLayout(controlsLayout), terrorSave(terrorSave), buttonTranslator(buttonTranslator), 
			mouseCursor(MouseCursor::getInstance()), buttonWasPressed(false) {}

		void bindKeyToCommand(const ButtonCode& buttonCode) {
			if (this->checkKeyValidity(buttonCode)) {
				this->setControlKeyCode(buttonCode);
				this->terrorSave->save();
			}
			this->updateButtonCaption();
			this->buttonWasPressed = false;
		}

		virtual void updateButtonCaption() {
			static_cast<MyGUI::Button*>(this->controlButton)->setCaption("");
		}

		bool wasButtonPressed() {
			return this->buttonWasPressed;
		}

		virtual ButtonCode getKeyCode() {
			return BC_UNASSIGNED;
		}

		void unassignCommandKey() {
			this->setControlKeyCode(BC_UNASSIGNED);
			this->terrorSave->save();
			this->updateButtonCaption();
		}

		virtual ~ControlButtonHandler() {}

	protected:
		TerrorSavePtr terrorSave;
		GuiLayoutPtr controlsLayout;
		Widget* controlButton;
		ButtonTranslatorPtr buttonTranslator;
		MouseCursorPtr mouseCursor;
		bool buttonWasPressed;

	private:
		virtual void handleButtonClick() {
			static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Press any key (or Esc to cancel)");
			this->mouseCursor->hide();
			this->controlsLayout->disable();
			this->buttonWasPressed = true;
		}

		virtual bool checkKeyValidity(const ButtonCode& buttonCode){
			return buttonCode != BC_ESCAPE;
		}

		virtual void setControlKeyCode(const ButtonCode& buttonCode) {}
	};

	typedef SharedPointer<ControlButtonHandler> ControlButtonHandlerPtr;
	typedef PointerList<ControlButtonHandler> ControlButtonHandlerPtrList;
	typedef SharedPointer<ControlButtonHandlerPtrList> ControlButtonHandlerPtrListPtr;
}

#endif
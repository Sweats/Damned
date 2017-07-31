#ifndef TERROR_TAB_BUTTON_HANDLER_H
#define TERROR_TAB_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class TabButtonHandler : public ControlButtonHandler {
	public:
		TabButtonHandler(Widget* tabButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(tabButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->tab;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->tab == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->tab));
		}

		virtual ~TabButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->tab = keyCode;
		}
	};

	typedef SharedPointer<TabButtonHandler> TabButtonHandlerPtr;
}

#endif
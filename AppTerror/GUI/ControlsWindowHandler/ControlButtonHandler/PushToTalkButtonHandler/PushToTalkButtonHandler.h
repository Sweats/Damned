#ifndef TERROR_PUSH_TO_TALK_BUTTON_HANDLER_H
#define TERROR_PUSH_TO_TALK_BUTTON_HANDLER_H

#include "../ControlButtonHandler.h"

namespace terror {
	class PushToTalkButtonHandler : public ControlButtonHandler {
	public:
		PushToTalkButtonHandler(Widget* pushToTalkButton, GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator)
			: ControlButtonHandler(pushToTalkButton, controlsLayout, terrorSave, buttonTranslator) 
		{
			this->updateButtonCaption();
		}

		ButtonCode getKeyCode() {
			return this->terrorSave->controls->pushToTalk;
		}

		void updateButtonCaption(){
			if (this->terrorSave->controls->pushToTalk == BC_UNASSIGNED)
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption("Unassigned");
			else
				static_cast<MyGUI::Button*>(this->controlButton)->setCaption(this->buttonTranslator->translateButton(this->terrorSave->controls->pushToTalk));
		}

		virtual ~PushToTalkButtonHandler() {}

	private:
		void setControlKeyCode(const ButtonCode& keyCode) {
			this->terrorSave->controls->pushToTalk = keyCode;
		}
	};

	typedef SharedPointer<PushToTalkButtonHandler> PushToTalkButtonHandlerPtr;
}

#endif
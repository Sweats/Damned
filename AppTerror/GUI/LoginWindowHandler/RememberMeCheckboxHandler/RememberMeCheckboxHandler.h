#ifndef TERROR_REMEMBER_ME_CHECKBOX_HANDLER_H
#define TERROR_REMEMBER_ME_CHECKBOX_HANDLER_H

#include "Config.h"

namespace terror {
	class RememberMeCheckboxHandler : public CheckboxHandler {
	public:
		RememberMeCheckboxHandler (Widget* rememberMeCheckbox) 
			: CheckboxHandler(rememberMeCheckbox) {}

		bool getCheckboxState() {
			return this->checkbox->getStateSelected();
		}

		virtual ~RememberMeCheckboxHandler() {}

	private:
		virtual void handleCheckbox(bool state) {}
	};
	typedef SharedPointer<RememberMeCheckboxHandler> RememberMeCheckboxHandlerPtr;
}

#endif
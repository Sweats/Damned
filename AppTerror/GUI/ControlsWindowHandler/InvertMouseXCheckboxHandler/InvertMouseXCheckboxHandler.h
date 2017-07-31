#ifndef TERROR_INVERT_MOUSE_X_CHECKBOX_HANDLER_H
#define TERROR_INVERT_MOUSE_X_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class InvertMouseXCheckboxHandler : public CheckboxHandler {
	public:
		InvertMouseXCheckboxHandler(Widget* invertMouseXCheckbox, TerrorSavePtr terrorSave) 
			: CheckboxHandler(invertMouseXCheckbox), terrorSave(terrorSave) 
		{
			if (this->terrorSave->controls->invertMouseX) {
				this->checkbox->setStateSelected(true);
			}
		}

		void updateCheckboxState() {
			this->checkbox->setStateSelected(this->terrorSave->controls->invertMouseX);
		}

		virtual ~InvertMouseXCheckboxHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->controls->invertMouseX = state;
			this->terrorSave->save();
		}
	};

	typedef SharedPointer<InvertMouseXCheckboxHandler> InvertMouseXCheckboxHandlerPtr;
}

#endif
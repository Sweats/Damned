#ifndef TERROR_INVERT_MOUSE_Y_CHECKBOX_HANDLER_H
#define TERROR_INVERT_MOUSE_Y_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class InvertMouseYCheckboxHandler : public CheckboxHandler {
	public:
		InvertMouseYCheckboxHandler(Widget* invertMouseYCheckbox, TerrorSavePtr terrorSave) 
			: CheckboxHandler(invertMouseYCheckbox), terrorSave(terrorSave) 
		{
			if (this->terrorSave->controls->invertMouseY) {
				this->checkbox->setStateSelected(true);
			}
		}

		void updateCheckboxState() {
			this->checkbox->setStateSelected(this->terrorSave->controls->invertMouseY);
		}

		virtual ~InvertMouseYCheckboxHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->controls->invertMouseY = state;
			this->terrorSave->save();
		}
	};

	typedef SharedPointer<InvertMouseYCheckboxHandler> InvertMouseYCheckboxHandlerPtr;
}

#endif
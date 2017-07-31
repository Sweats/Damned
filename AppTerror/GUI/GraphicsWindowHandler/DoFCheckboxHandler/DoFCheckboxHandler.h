#ifndef TERROR_DOF_CHECKBOX_HANDLER_H
#define TERROR_DOF_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class DoFCheckboxHandler : public CheckboxHandler {
	public:
		DoFCheckboxHandler(Widget* dofCheckbox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
			: CheckboxHandler(dofCheckbox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
		{
			if (this->terrorSave->dof) {
				this->deferredShadingSystem->enableDoF();				
			}else {
				this->deferredShadingSystem->disableDoF();
			}
			this->setCheckboxState(this->terrorSave->dof);
		}

		virtual ~DoFCheckboxHandler() {}

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->dof = state;
			this->terrorSave->save();
			if (this->terrorSave->dof) {
				this->deferredShadingSystem->enableDoF();
			}else {
				this->deferredShadingSystem->disableDoF();
			}			
		}
	};

	typedef SharedPointer<DoFCheckboxHandler> DoFCheckboxHandlerPtr;
}

#endif
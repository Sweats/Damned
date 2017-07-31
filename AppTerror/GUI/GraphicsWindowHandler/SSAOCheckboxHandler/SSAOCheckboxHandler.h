#ifndef TERROR_SSAO_CHECKBOX_HANDLER_H
#define TERROR_SSAO_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class SSAOCheckboxHandler : public CheckboxHandler {
	public:
		SSAOCheckboxHandler(Widget* ssaoCheckbox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
			: CheckboxHandler(ssaoCheckbox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
		{
			if (this->terrorSave->ssao) {
				this->deferredShadingSystem->enableSSAO();
			}else {
				this->deferredShadingSystem->disableSSAO();
			}
			this->setCheckboxState(this->terrorSave->ssao);
		}

		virtual ~SSAOCheckboxHandler() {}

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->ssao = state;
			this->terrorSave->save();
			if (this->terrorSave->ssao) {
				this->deferredShadingSystem->enableSSAO();
			}else {
				this->deferredShadingSystem->disableSSAO();
			}
		}
	};

	typedef SharedPointer<SSAOCheckboxHandler> SSAOCheckboxHandlerPtr;
}

#endif
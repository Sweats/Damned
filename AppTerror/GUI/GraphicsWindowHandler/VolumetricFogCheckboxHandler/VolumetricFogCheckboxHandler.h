#ifndef TERROR_VOLUMETRIC_FOG_CHECKBOX_HANDLER_H
#define TERROR_VOLUMETRIC_FOG_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class VolumetricFogCheckboxHandler : public CheckboxHandler {
	public:
		VolumetricFogCheckboxHandler(Widget* volumetricFogCheckbox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
			: CheckboxHandler(volumetricFogCheckbox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
		{
			if (this->terrorSave->volumetricFog) {
				this->deferredShadingSystem->enableVolumetricFog();
			}else {
				this->deferredShadingSystem->disableVolumetricFog();
			}
			this->setCheckboxState(this->terrorSave->volumetricFog);
		}

		virtual ~VolumetricFogCheckboxHandler() {}

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->volumetricFog = state;
			this->terrorSave->save();
			if (this->terrorSave->volumetricFog) {
				this->deferredShadingSystem->enableVolumetricFog();
			}else {
				this->deferredShadingSystem->disableVolumetricFog();
			}
		}
	};

	typedef SharedPointer<VolumetricFogCheckboxHandler> VolumetricFogCheckboxHandlerPtr;
}

#endif
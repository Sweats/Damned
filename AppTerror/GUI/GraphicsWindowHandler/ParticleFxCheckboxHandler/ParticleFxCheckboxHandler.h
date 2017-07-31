#ifndef TERROR_PARTICLE_FX_CHECKBOX_HANDLER_H
#define TERROR_PARTICLE_FX_CHECKBOX_HANDLER_H

#include "ParticleFxHandler/ParticleFxHandler.h"

namespace terror {
	class ParticleFxCheckboxHandler : public CheckboxHandler {
	public:
		ParticleFxCheckboxHandler(Widget* particleFxCheckbox, ParticleFxHandlerPtr particleFxHandler, TerrorSavePtr terrorSave) 
			: CheckboxHandler(particleFxCheckbox), particleFxHandler(particleFxHandler), terrorSave(terrorSave) 
		{
			this->setCheckboxState(this->terrorSave->particleFx);
		}

		virtual ~ParticleFxCheckboxHandler() {}

	protected:
		ParticleFxHandlerPtr particleFxHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->particleFx = state;
			this->terrorSave->save();

			if (this->terrorSave->particleFx) {
				this->particleFxHandler->enable();
			}else {
				this->particleFxHandler->disable();
			}
		}
	};

	typedef SharedPointer<ParticleFxCheckboxHandler> ParticleFxCheckboxHandlerPtr;
}

#endif
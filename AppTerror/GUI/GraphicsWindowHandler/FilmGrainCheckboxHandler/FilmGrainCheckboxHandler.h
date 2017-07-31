#ifndef TERROR_FILM_GRAIN_CHECKBOX_HANDLER_H
#define TERROR_FILM_GRAIN_CHECKBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class FilmGrainCheckboxHandler : public CheckboxHandler {
	public:
		FilmGrainCheckboxHandler(Widget* filmGrainCheckbox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
			: CheckboxHandler(filmGrainCheckbox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
		{
			if (this->terrorSave->filmGrain) {
				this->deferredShadingSystem->enableFilmGrain();
			}else {
				this->deferredShadingSystem->disableFilmGrain();
			}
			this->setCheckboxState(this->terrorSave->filmGrain);
		}

		virtual ~FilmGrainCheckboxHandler() {}

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCheckbox(bool state) {
			this->terrorSave->filmGrain = state;
			this->terrorSave->save();
			if (this->terrorSave->filmGrain) {
				this->deferredShadingSystem->enableFilmGrain();
			}else {
				this->deferredShadingSystem->disableFilmGrain();
			}
		}
	};

	typedef SharedPointer<FilmGrainCheckboxHandler> FilmGrainCheckboxHandlerPtr;
}

#endif
#ifndef TERROR_GAMMA_SLIDER_HANDLER_H
#define TERROR_GAMMA_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class GammaSliderHandler : public SliderHandler {
	public:
		GammaSliderHandler(Widget* gammaScrollbar, MyGUI::TextBox* gammaTextbox, DeferredShadingSystemPtr deferredShadingSystem, 
			TerrorSavePtr terrorSave)
			: SliderHandler(gammaScrollbar), gammaTextbox(gammaTextbox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
		{		
			this->setGammaText(this->terrorSave->gamma);
			this->deferredShadingSystem->setGamma(this->terrorSave->gamma);

			int scrollbarPosition = (this->terrorSave->gamma - 0.5f)/0.05f;
			this->setSliderPosition(scrollbarPosition);
		}

		virtual ~GammaSliderHandler() {}

	protected:
		MyGUI::TextBox* gammaTextbox;
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->gamma = 0.5f + 0.05f*static_cast<float>(position);
			this->terrorSave->save();

			this->setGammaText(this->terrorSave->gamma);
			this->deferredShadingSystem->setGamma(this->terrorSave->gamma);			
		}

		void setGammaText(float gamma) {
			std::stringstream stream;
			stream.precision(3);
			stream << gamma;
			std::string gammaString;
			stream >> gammaString;
			this->gammaTextbox->setCaption(gammaString);
		}
	};

	typedef SharedPointer<GammaSliderHandler> GammaSliderHandlerPtr;
}

#endif
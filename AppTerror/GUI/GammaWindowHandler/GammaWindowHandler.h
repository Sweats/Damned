#ifndef TERROR_GAMMA_WINDOW_HANDLER_H
#define TERROR_GAMMA_WINDOW_HANDLER_H

#include "GammaSliderHandler/GammaSliderHandler.h"

namespace terror {
	class GammaWindowHandler : public InputHandler {
	public:
		GammaWindowHandler(GuiLayoutPtr gammaLayout, PostProcessingSystemPtr postProcessingSystem, 
			DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
			: postProcessingSystem(postProcessingSystem), gammaLayout(gammaLayout), terrorSave(terrorSave)
		{
			Widget* gammaSlider = gammaLayout->getWidget("GammaScrollbar");
			MyGUI::TextBox* gammaTextbox = static_cast<MyGUI::TextBox*>(gammaLayout->getWidget("GammaTextbox"));
			this->gammaSliderHandler = 
				GammaSliderHandlerPtr(new GammaSliderHandler(gammaSlider, gammaTextbox, deferredShadingSystem, terrorSave));
			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(gammaLayout->getWidget("BackButton"), gammaLayout));

			gammaLayout->connectShowEvent(Bind(&GammaWindowHandler::startGammaAdjustmentEffect, this));
			gammaLayout->connectHideEvent(Bind(&GammaWindowHandler::stopGammaAdjustmentEffect, this));
		}

		virtual ~GammaWindowHandler() {}

	protected:
		GuiLayoutPtr gammaLayout;
		TerrorSavePtr terrorSave;
		GammaSliderHandlerPtr gammaSliderHandler;
		BackButtonHandlerPtr backButtonHandler;
		PostProcessingSystemPtr postProcessingSystem;
		PostProcessingEffectPtr gammaAdjustmentEffect;

	private:
		void startGammaAdjustmentEffect() {
			this->gammaAdjustmentEffect = this->postProcessingSystem->addPostProcessingEffect("GammaAdjustment");
			this->gammaAdjustmentEffect->setEnabled(true);
		}

		void stopGammaAdjustmentEffect() {
			this->postProcessingSystem->removePostProcessingEffect("GammaAdjustment");
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->gammaLayout->isVisible()) {
				this->gammaLayout->hide();
			}
		}
	};

	typedef SharedPointer<GammaWindowHandler> GammaWindowHandlerPtr;
}

#endif
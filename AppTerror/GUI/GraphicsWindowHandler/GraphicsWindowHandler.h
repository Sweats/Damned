#ifndef TERROR_GRAPHICS_WINDOW_HANDLER_H
#define TERROR_GRAPHICS_WINDOW_HANDLER_H

#include "ResolutionsComboboxHandler/ResolutionsComboboxHandler.h"
#include "AnisotropicFilteringComboboxHandler/AnisotropicFilteringComboboxHandler.h"
#include "ShadowQualityComboboxHandler/ShadowQualityComboboxHandler.h"
#include "FxaaComboboxHandler/FxaaComboboxHandler.h"
#include "FullscreenCheckboxHandler/FullscreenCheckboxHandler.h"
#include "VSyncCheckboxHandler/VSyncCheckboxHandler.h"
#include "DoFCheckboxHandler/DoFCheckboxHandler.h"
#include "SSAOCheckboxHandler/SSAOCheckboxHandler.h"
#include "ParticleFxCheckboxHandler/ParticleFxCheckboxHandler.h"
#include "VolumetricFogCheckboxHandler/VolumetricFogCheckboxHandler.h"
#include "FilmGrainCheckboxHandler/FilmGrainCheckboxHandler.h"

namespace terror {
	class GraphicsWindowHandler : public InputHandler {
	public:
		GraphicsWindowHandler(GuiLayoutPtr graphicsLayout, GuiLayoutPtr gammaLayout, GraphicsOptionsPtr graphicsOptions, 
			DeferredShadingSystemPtr deferredShadingSystem, SceneManagerPtr sceneManager, ParticleFxHandlerPtr particleFxHandler, TerrorSavePtr terrorSave) 
			: graphicsLayout(graphicsLayout), terrorSave(terrorSave)
		{
			Widget* resolutionsCombobox = graphicsLayout->getWidget("ResolutionsCombobox");
			this->resolutionsComboboxHandler = ResolutionsComboboxHandlerPtr(
				new ResolutionsComboboxHandler(resolutionsCombobox, graphicsOptions));

			Widget* anisotropicFilteringCombobox = graphicsLayout->getWidget("AnisotropicFilteringCombobox");
			this->anisotropicFilteringComboboxHandler = AnisotropicFilteringComboboxHandlerPtr(
				new AnisotropicFilteringComboboxHandler(anisotropicFilteringCombobox, terrorSave));

			Widget* shadowQualityCombobox = graphicsLayout->getWidget("ShadowQualityCombobox");
			this->shadowQualityComboboxHandler = ShadowQualityComboboxHandlerPtr(
				new ShadowQualityComboboxHandler(shadowQualityCombobox, sceneManager, terrorSave));

			Widget* fxaaCombobox = graphicsLayout->getWidget("FxaaCombobox");
			this->fxaaComboboxHandler = FxaaComboboxHandlerPtr(
				new FxaaComboboxHandler(fxaaCombobox, deferredShadingSystem, terrorSave));

			this->vSyncCheckboxHandler = VSyncCheckboxHandlerPtr(
				new VSyncCheckboxHandler(graphicsLayout->getWidget("VSyncCheckbox"), graphicsOptions));

			this->fullscreenCheckboxHandler = FullscreenCheckboxHandlerPtr(
				new FullscreenCheckboxHandler(graphicsLayout->getWidget("FullscreenCheckbox"), graphicsOptions));

			this->particleFxCheckboxHandler = ParticleFxCheckboxHandlerPtr(
				new ParticleFxCheckboxHandler(graphicsLayout->getWidget("ParticleFxCheckbox"), particleFxHandler, terrorSave));

			this->dofCheckboxHandler = 
				DoFCheckboxHandlerPtr(new DoFCheckboxHandler(graphicsLayout->getWidget("DepthOfFieldCheckbox"), deferredShadingSystem, terrorSave));

			this->ssaoCheckboxHandler = 
				SSAOCheckboxHandlerPtr(new SSAOCheckboxHandler(graphicsLayout->getWidget("AmbientOcclusionCheckbox"), deferredShadingSystem, terrorSave));

			this->volumetricFogCheckboxHandler = 
				VolumetricFogCheckboxHandlerPtr(new VolumetricFogCheckboxHandler(graphicsLayout->getWidget("VolumetricFogCheckbox"), deferredShadingSystem, terrorSave));

			this->filmGrainCheckboxHandler = 
				FilmGrainCheckboxHandlerPtr(new FilmGrainCheckboxHandler(graphicsLayout->getWidget("FilmGrainCheckbox"), deferredShadingSystem, terrorSave));

			this->gammaButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(graphicsLayout->getWidget("GammaButton"), graphicsLayout, gammaLayout));

			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(graphicsLayout->getWidget("BackButton"), graphicsLayout));
		}

		virtual ~GraphicsWindowHandler() {}

	protected:
		GuiLayoutPtr graphicsLayout;
		TerrorSavePtr terrorSave;
		ResolutionsComboboxHandlerPtr resolutionsComboboxHandler;
		AnisotropicFilteringComboboxHandlerPtr anisotropicFilteringComboboxHandler;
		ShadowQualityComboboxHandlerPtr shadowQualityComboboxHandler;
		FxaaComboboxHandlerPtr fxaaComboboxHandler;
		VSyncCheckboxHandlerPtr vSyncCheckboxHandler;
		FullscreenCheckboxHandlerPtr fullscreenCheckboxHandler;
		DoFCheckboxHandlerPtr dofCheckboxHandler;
		SSAOCheckboxHandlerPtr ssaoCheckboxHandler;
		VolumetricFogCheckboxHandlerPtr volumetricFogCheckboxHandler;
		FilmGrainCheckboxHandlerPtr filmGrainCheckboxHandler;
		ParticleFxCheckboxHandlerPtr particleFxCheckboxHandler;
		WindowCallerButtonHandlerPtr gammaButtonHandler;
		BackButtonHandlerPtr backButtonHandler;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->graphicsLayout->isVisible()) {
				this->graphicsLayout->hide();
			}
		}
	};

	typedef SharedPointer<GraphicsWindowHandler> GraphicsWindowHandlerPtr;
}

#endif
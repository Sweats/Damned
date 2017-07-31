#ifndef TERROR_SHADOW_QUALITY_COMBOBOX_HANDLER_H
#define TERROR_SHADOW_QUALITY_COMBOBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class ShadowQualityComboboxHandler : public ComboboxHandler {
	public:
		ShadowQualityComboboxHandler(Widget* shadowQualityCombobox, SceneManagerPtr sceneManager, TerrorSavePtr terrorSave) 
			: ComboboxHandler(shadowQualityCombobox), sceneManager(sceneManager), terrorSave(terrorSave) 
		{
			int shadowAntiAliasing = 0;
			int shadowDepthPoolId = 2;
			this->sceneManager->setShadowTextureConfig(0, this->terrorSave->shadowQuality, this->terrorSave->shadowQuality, 
				Ogre::PF_FLOAT16_R, shadowAntiAliasing, shadowDepthPoolId);
			
			StringList shadowQualities = this->terrorSave->getPossibleShadowQualityValues();
			foreach (string shadowQuality, shadowQualities) {
				this->addComboboxItem(shadowQuality);
			}
			this->setSelectedComboboxItem(this->terrorSave->getShadowQualityString());
		}

		virtual ~ShadowQualityComboboxHandler() {}

	protected:
		SceneManagerPtr sceneManager;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCombobox(const string& selected) {
			this->terrorSave->setShadowQuality(selected);
			this->terrorSave->save();

			int shadowAntiAliasing = 0;
			int shadowDepthPoolId = 2;
			this->sceneManager->setShadowTextureConfig(0, this->terrorSave->shadowQuality, this->terrorSave->shadowQuality, 
				Ogre::PF_FLOAT16_R, shadowAntiAliasing, shadowDepthPoolId);
		}
	};

	typedef SharedPointer<ShadowQualityComboboxHandler> ShadowQualityComboboxHandlerPtr;
}

#endif
#include "FxaaComboboxHandler.h"

namespace terror {
	FxaaComboboxHandler::FxaaComboboxHandler(Widget* fxaaCombobox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave) 
		: ComboboxHandler(fxaaCombobox), deferredShadingSystem(deferredShadingSystem), terrorSave(terrorSave) 
	{
		this->setFxaa(this->terrorSave->fxaaQuality);	
		StringList fxaaValues = this->terrorSave->getPossibleFxaaValues();
		foreach (string fxaaValue, fxaaValues) {
			this->addComboboxItem(fxaaValue);
		}
		this->setSelectedComboboxItem(this->terrorSave->getFxaaString());
	}

	FxaaComboboxHandler::~FxaaComboboxHandler() {}

	void FxaaComboboxHandler::handleCombobox(const string& selected) {
		this->terrorSave->setFxaa(selected);
		this->terrorSave->save();
		this->setFxaa(this->terrorSave->fxaaQuality);
	}

	void FxaaComboboxHandler::setFxaa(int fxaaQuality) {
		if (fxaaQuality == 0) {
			this->deferredShadingSystem->disableFXAA();
			return;
		}		
		this->deferredShadingSystem->enableFXAA();
		switch (fxaaQuality) {
			case 1:
				this->deferredShadingSystem->setFxaaParameters(1.0f/3.0f, 1.0f/12.0f, 0.5f);
				break;
			case 2:
				this->deferredShadingSystem->setFxaaParameters(0.25f, 1.0f/12.0f, 0.5f);
				break;
			case 3:
				this->deferredShadingSystem->setFxaaParameters(1.0f/6.0f, 1.0f/12.0f, 0.75f);
				break;
			case 4:
				this->deferredShadingSystem->setFxaaParameters(1.0f/8.0f, 1.0f/16.0f, 0.75f);
				break;
			case 5:
				this->deferredShadingSystem->setFxaaParameters(1.0f/16.0f, 1.0f/32.0f, 1.0f);
				break;
		}
	}
}
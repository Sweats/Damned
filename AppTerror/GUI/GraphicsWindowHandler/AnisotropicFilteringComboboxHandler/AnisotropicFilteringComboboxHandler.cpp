#include "AnisotropicFilteringComboboxHandler.h"

namespace terror {
	AnisotropicFilteringComboboxHandler::AnisotropicFilteringComboboxHandler(Widget* anisotropicFilteringCombobox, TerrorSavePtr terrorSave) 
		: ComboboxHandler(anisotropicFilteringCombobox), terrorSave(terrorSave) 
	{
		if (this->terrorSave->anisotropicFiltering == 0) {
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
		}else {
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
			Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(this->terrorSave->anisotropicFiltering);
		}

		this->addComboboxItem("0");
		this->addComboboxItem("1");
		this->addComboboxItem("2");
		this->addComboboxItem("4");
		this->addComboboxItem("8");
		this->addComboboxItem("16");
		this->setSelectedComboboxItem(lexical_cast<std::string>(this->terrorSave->anisotropicFiltering));
	}

	AnisotropicFilteringComboboxHandler::~AnisotropicFilteringComboboxHandler() {}

	void AnisotropicFilteringComboboxHandler::handleCombobox(const string& selected) {
		this->terrorSave->anisotropicFiltering = lexical_cast<int>(selected);
		this->terrorSave->save();
		if (this->terrorSave->anisotropicFiltering == 0) {
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
		}else {
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
			Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(this->terrorSave->anisotropicFiltering);
		}		
	}
}
#include "TerrorPassProperties.h"

namespace terror_deferred_shading {
	TerrorPassProperties::TerrorPassProperties() 
		: diffuseMap(0), normalMap(0), specularMap(0), emissiveMap(0), decalDiffuseMap(0), decalNormalMap(0), decalSpecularMap(0), 
		decalDiffuseMap2(0), decalNormalMap2(0), decalSpecularMap2(0), dxtNormal(false), decalDxtNormal(false), decalDxtNormal2(false) {}

	TerrorPassProperties::~TerrorPassProperties() {}

	void TerrorPassProperties::initialize(Pass* pass) {
		for (int i = 0; i < pass->getNumTextureUnitStates(); i++) {
			Ogre::TextureUnitState* textureUnitState = pass->getTextureUnitState(i);
			this->assignTexture(textureUnitState);		
		}
		this->ambient = pass->getAmbient();
		this->diffuse = pass->getDiffuse();
		this->specular = pass->getSpecular();
		this->shininess = pass->getShininess();
		this->cullingMode = pass->getCullingMode();
	}

	void TerrorPassProperties::fillPass(Pass* pass) {
		int texUnitIndex = 0;

		*(pass->getTextureUnitState(texUnitIndex)) = *(this->diffuseMap);
		texUnitIndex++;

		if (this->normalMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->normalMap);
			texUnitIndex++;
		}
		if (this->specularMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->specularMap);
			texUnitIndex++;
		}
		if (this->emissiveMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->emissiveMap);
			texUnitIndex++;
		}
		if (this->decalDiffuseMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalDiffuseMap);
			texUnitIndex++;
		}
		if (this->decalNormalMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalNormalMap);
			texUnitIndex++;
		}
		if (this->decalSpecularMap != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalSpecularMap);
			texUnitIndex++;
		}
		if (this->decalDiffuseMap2 != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalDiffuseMap2);
			texUnitIndex++;
		}
		if (this->decalNormalMap2 != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalNormalMap2);
			texUnitIndex++;
		}
		if (this->decalSpecularMap2 != 0) {
			*(pass->getTextureUnitState(texUnitIndex)) = *(this->decalSpecularMap2);
			texUnitIndex++;
		}
		pass->setAmbient(this->ambient);
		pass->setDiffuse(this->diffuse);
		pass->setSpecular(this->specular);
		pass->setShininess(this->shininess);
		pass->setCullingMode(this->cullingMode);
		pass->setLightingEnabled(false);
	}

	void TerrorPassProperties::assignTexture(Ogre::TextureUnitState* textureUnitState) {
		if (textureUnitState->getName() == "diffuseMap") {
			this->diffuseMap = textureUnitState;
			return;
		}
		if (textureUnitState->getName() == "normalMap") {				
			this->normalMap = textureUnitState;
			this->permutation.add(GBP_NORMAL_MAP);
			if (textureUnitState->getTextureName().find(".dds") == Ogre::String::npos) {
				this->dxtNormal = false;
				return;
			}
			this->dxtNormal = true;
			this->permutation.add(GBP_DXT_NORMAL);
			return;
		}
		if (textureUnitState->getName() == "specularMap") {
			this->specularMap = textureUnitState;
			this->permutation.add(GBP_SPECULAR_MAP);
			return;
		}
		if (textureUnitState->getName() == "emissiveMap") {
			this->emissiveMap = textureUnitState;
			this->permutation.add(GBP_EMISSIVE_MAP);
			return;
		}
		if (textureUnitState->getName() == "decalDiffuseMap") {
			this->decalDiffuseMap = textureUnitState;
			this->permutation.add(GBP_DECAL_DIFFUSE_MAP);
			return;
		}
		if (textureUnitState->getName() == "decalNormalMap") {
			this->decalNormalMap = textureUnitState;
			this->permutation.add(GBP_DECAL_NORMAL_MAP);
			if (textureUnitState->getTextureName().find(".dds") == Ogre::String::npos) {
				this->decalDxtNormal = false;
				return;
			}
			this->decalDxtNormal = true;
			this->permutation.add(GBP_DECAL_DXT_NORMAL);
			return;
		}
		if (textureUnitState->getName() == "decalSpecularMap") {
			this->decalSpecularMap = textureUnitState;
			this->permutation.add(GBP_DECAL_SPECULAR_MAP);
			return;
		}

		if (textureUnitState->getName() == "decalDiffuseMap2") {
			this->decalDiffuseMap2 = textureUnitState;
			this->permutation.add(GBP_DECAL_DIFFUSE_MAP_2);
			return;
		}
		if (textureUnitState->getName() == "decalNormalMap2") {
			this->decalNormalMap2 = textureUnitState;
			this->permutation.add(GBP_DECAL_NORMAL_MAP_2);
			if (textureUnitState->getTextureName().find(".dds") == Ogre::String::npos) {
				this->decalDxtNormal2 = false;
				return;
			}
			this->decalDxtNormal2 = true;
			this->permutation.add(GBP_DECAL_DXT_NORMAL_2);
			return;
		}
		if (textureUnitState->getName() == "decalSpecularMap2") {
			this->decalSpecularMap2 = textureUnitState;
			this->permutation.add(GBP_DECAL_SPECULAR_MAP_2);
			return;
		}
	}
}

#ifndef TERROR_DEFERRED_SHADING_TERROR_PASS_PROPERTIES_H
#define TERROR_DEFERRED_SHADING_TERROR_PASS_PROPERTIES_H

#include "Config.h"

namespace terror_deferred_shading {
	enum TerrorGBufferPermutations {
		GBP_NORMAL_MAP =			0x00000001,
		GBP_DXT_NORMAL =			0x00000002,
		GBP_SPECULAR_MAP =			0x00000004,
		GBP_EMISSIVE_MAP =			0x00000008,
		GBP_DECAL_DIFFUSE_MAP =		0x00000010,
		GBP_DECAL_NORMAL_MAP =		0x00000020,
		GBP_DECAL_DXT_NORMAL =		0x00000040,
		GBP_DECAL_SPECULAR_MAP =	0x00000080,
		GBP_DECAL_DIFFUSE_MAP_2 =	0x00000100,
		GBP_DECAL_NORMAL_MAP_2 =	0x00000200,
		GBP_DECAL_DXT_NORMAL_2 =	0x00000400,
		GBP_DECAL_SPECULAR_MAP_2 =	0x00000800,

		VS_MASK =					0x00000FFF,
		FS_MASK =					0x00000FFF,
		MAT_MASK =					0xFF000FFF,
	};

	class TerrorPassProperties : public PassProperties {
	public:
		TerrorPassProperties();

		virtual ~TerrorPassProperties();

	protected:
		Ogre::TextureUnitState* diffuseMap;
		Ogre::TextureUnitState* normalMap;
		Ogre::TextureUnitState* specularMap;
		Ogre::TextureUnitState* emissiveMap;
		Ogre::TextureUnitState* decalDiffuseMap;
		Ogre::TextureUnitState* decalNormalMap;
		Ogre::TextureUnitState* decalSpecularMap;
		Ogre::TextureUnitState* decalDiffuseMap2;
		Ogre::TextureUnitState* decalNormalMap2;
		Ogre::TextureUnitState* decalSpecularMap2;
		ColourValue ambient;
		ColourValue diffuse;
		ColourValue specular;
		float shininess;
		Ogre::CullingMode cullingMode;
		bool dxtNormal;
		bool decalDxtNormal;
		bool decalDxtNormal2;

	private:
		virtual void initialize(Pass* pass);
		virtual void fillPass(Pass* pass);

		void assignTexture(Ogre::TextureUnitState* textureUnitState);
	};

	typedef SharedPointer<TerrorPassProperties> TerrorPassPropertiesPtr;
}

#endif

#include "TerrorMaterialGeneratorImpl.h"

namespace terror_deferred_shading {
	TerrorMaterialGeneratorImpl::TerrorMaterialGeneratorImpl() {}		

	TerrorMaterialGeneratorImpl::~TerrorMaterialGeneratorImpl() {}

	Permutation TerrorMaterialGeneratorImpl::getVertexShaderMask() {
		return VS_MASK;
	}

	Permutation TerrorMaterialGeneratorImpl::getFragmentShaderMask() {
		return FS_MASK;
	}

	Permutation TerrorMaterialGeneratorImpl::getMaterialMask() {
		return MAT_MASK;
	}

	Ogre::GpuProgramPtr TerrorMaterialGeneratorImpl::generateVertexShader(Permutation permutation) {
		Ogre::StringStream ss;
		ss << "void toGBuffer_vp(" << std::endl;
		ss << "	float4 position : POSITION," << std::endl;
		ss << "	float2 uv0 : TEXCOORD0," << std::endl;
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << "	float2 uv1 : TEXCOORD1," << std::endl;
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << "	float2 uv2 : TEXCOORD2," << std::endl;
		}
		ss << "	float3 normal : NORMAL," << std::endl;
		if (permutation.has(GBP_NORMAL_MAP)) {
			ss << "	float4 tangent : TANGENT," << std::endl;
		}
		ss << std::endl;

		ss << "	out float4 outPosition : POSITION," << std::endl;
		int texCoordNum = 0;
		ss << "	out float3 outPosition_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		ss << "	out float2 outUv0 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << "	out float2 outUv1 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << "	out float2 outUv2 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		ss << "	out float3 outNormal_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;	
		if (permutation.has(GBP_NORMAL_MAP)) {
			ss << "	out float3 outTangent_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
			ss << "	out float3 outBinormal_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		ss << std::endl;
		ss << "	uniform float4x4 worldView," << std::endl;
		ss << "	uniform float4x4 worldViewProj" << std::endl;
		ss << "	)" << std::endl;	
		ss << "{" << std::endl;		
		ss << "	outPosition = mul(worldViewProj, position);" << std::endl;
		ss << "	outPosition_VS = mul(worldView, position).xyz;" << std::endl;
		ss << "	outUv0 = uv0;" << std::endl;
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << "	outUv1 = uv1;" << std::endl;
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << "	outUv2 = uv2;" << std::endl;
		}
		ss << "	outNormal_VS = mul(worldView, float4(normal, 0.0)).xyz;" << std::endl;
		if (permutation.has(GBP_NORMAL_MAP)) {
			ss << "	outTangent_VS = mul(worldView, float4(tangent.xyz, 0.0)).xyz;" << std::endl;
			ss << "	outBinormal_VS = -tangent.w*cross(outTangent_VS, outNormal_VS);" << std::endl;
		}   
		ss << "}" << std::endl;

		Ogre::String programSource = ss.str();
		Ogre::String programName = "VP_" + Ogre::StringConverter::toString(permutation);

		Ogre::HighLevelGpuProgramPtr program = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,	"cg", Ogre::GPT_VERTEX_PROGRAM);
		program->setSource(programSource);
		program->setParameter("entry_point", "toGBuffer_vp");
		program->setParameter("profiles", "vs_3_0");
		const Ogre::GpuProgramParametersSharedPtr& params = program->getDefaultParameters();
		params->setNamedAutoConstant("worldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
		params->setNamedAutoConstant("worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		program->load();
		return Ogre::GpuProgramPtr(program);
	}

	Ogre::GpuProgramPtr TerrorMaterialGeneratorImpl::generateFragmentShader(Permutation permutation) {
		Ogre::StringStream ss;
		ss << "void toGBuffer_fp(" << std::endl;
		int texCoordNum = 0;
		ss << "	float3 position_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		ss << "	float2 uv0 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << "	float2 uv1 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << "	float2 uv2 : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		ss << "	float3 normal_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;	
		if (permutation.has(GBP_NORMAL_MAP)) {
			ss << "	float3 tangent_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
			ss << "	float3 binormal_VS : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		ss << std::endl;
		ss << "	out float4 gBuffer0 : COLOR0," << std::endl;
		ss << "	out float4 gBuffer1 : COLOR1," << std::endl;
		ss << "	out float4 gBuffer2 : COLOR2," << std::endl;
		ss << std::endl;
		ss << "	uniform float4 diffuseColor," << std::endl;
		ss << "	uniform float farDistance," << std::endl;
		ss << "	uniform float shininess," << std::endl;
		ss << "	uniform float4 custom," << std::endl;
		int samplerNum = 0;
		ss << "\n	uniform sampler diffuseMap : register(s" << samplerNum++ << ")";
		if (permutation.has(GBP_NORMAL_MAP)) {
			ss << ",\n	uniform sampler normalMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_SPECULAR_MAP)) {
			ss << ",\n	uniform sampler specularMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_EMISSIVE_MAP)) {
			ss << ",\n	uniform sampler emissiveMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << ",\n	uniform sampler decalDiffuseMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_NORMAL_MAP)) {
			ss << ",\n	uniform sampler decalNormalMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_SPECULAR_MAP)) {
			ss << ",\n	uniform sampler decalSpecularMap : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << ",\n	uniform sampler decalDiffuseMap2 : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_NORMAL_MAP_2)) {
			ss << ",\n	uniform sampler decalNormalMap2 : register(s" << samplerNum++ << ")";
		}
		if (permutation.has(GBP_DECAL_SPECULAR_MAP_2)) {
			ss << ",\n	uniform sampler decalSpecularMap2 : register(s" << samplerNum++ << ")";
		}
		ss << "	)" << std::endl;
		ss << "{" << std::endl;
		ss << "	gBuffer0.rgb = tex2D(diffuseMap, uv0).rgb;" << std::endl;
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			ss << "	float4 decalDiffuse = tex2D(decalDiffuseMap, uv1);" << std::endl;
			ss << "	gBuffer0.rgb = lerp(gBuffer0.rgb, decalDiffuse.rgb, decalDiffuse.a);" << std::endl;
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			ss << "	float4 decalDiffuse2 = tex2D(decalDiffuseMap2, uv2);" << std::endl;
			ss << "	gBuffer0.rgb = lerp(gBuffer0.rgb, decalDiffuse2.rgb, decalDiffuse2.a);" << std::endl;
		}
		ss << "	gBuffer0.rgb *= diffuseColor.rgb;" << std::endl;				
		ss << "	gBuffer0.a = length(position_VS)/farDistance;" << std::endl;
		if (permutation.has(GBP_NORMAL_MAP)) {
			if (permutation.has(GBP_DXT_NORMAL)) {
				ss << "	float3 normal_TS = 2.0*tex2D(normalMap, uv0).wyy - 1.0;" << std::endl;
				ss << "	normal_TS.z = sqrt(1.0 - dot(normal_TS.xy, normal_TS.xy));" << std::endl;
			}else {
				ss << "	float3 normal_TS = 2.0*tex2D(normalMap, uv0).rgb - 1.0;" << std::endl;
			}
			ss << "	float3x3 tbnMatrix = float3x3(normalize(tangent_VS), normalize(binormal_VS), normalize(normal_VS));" << std::endl;
			ss << "	gBuffer1.rgb = mul(normal_TS, tbnMatrix);" << std::endl;
			if (permutation.has(GBP_DECAL_NORMAL_MAP)) {
				if (permutation.has(GBP_DECAL_DXT_NORMAL)) {
					ss << "	float3 decalNormal_TS = 2.0*tex2D(decalNormalMap, uv1).wyy - 1.0;" << std::endl;
					ss << "	decalNormal_TS.z = sqrt(1.0 - dot(decalNormal_TS.xy, decalNormal_TS.xy));" << std::endl;
				}else {
					ss << "	float3 decalNormal_TS = 2.0*tex2D(decalNormalMap, uv1).rgb - 1.0;" << std::endl;
				}
				ss << "	gBuffer1.rgb = lerp(gBuffer1.rgb, mul(decalNormal_TS, tbnMatrix), decalDiffuse.a);" << std::endl;
			}
			if (permutation.has(GBP_DECAL_NORMAL_MAP_2)) {				
				if (permutation.has(GBP_DECAL_DXT_NORMAL_2)) {
					ss << "	float3 decalNormal2_TS = 2.0*tex2D(decalNormalMap2, uv2).wyy - 1.0;" << std::endl;
					ss << "	decalNormal2_TS.z = sqrt(1.0 - dot(decalNormal2_TS.xy, decalNormal2_TS.xy));" << std::endl;
				}else {
					ss << "	float3 decalNormal2_TS = 2.0*tex2D(decalNormalMap2, uv2).rgb - 1.0;" << std::endl;
				}
				ss << "	gBuffer1.rgb = lerp(gBuffer1.rgb, mul(decalNormal2_TS, tbnMatrix), decalDiffuse2.a);" << std::endl;
			}
		}else {
			ss << "	gBuffer1.rgb = normalize(normal_VS);" << std::endl;
		}
		if (permutation.has(GBP_SPECULAR_MAP)) {		
			ss << "	gBuffer1.a = tex2D(specularMap, uv0).g;" << std::endl;
			if (permutation.has(GBP_DECAL_SPECULAR_MAP)) {
				ss << "	float decalSpecular = tex2D(decalSpecularMap, uv1).g;" << std::endl;
				ss << "	gBuffer1.a = lerp(gBuffer1.a, decalSpecular, decalDiffuse.a);" << std::endl;
			}
			if (permutation.has(GBP_DECAL_SPECULAR_MAP_2)) {
				ss << "	float decalSpecular2 = tex2D(decalSpecularMap2, uv2).g;" << std::endl;
				ss << "	gBuffer1.a = lerp(gBuffer1.a, decalSpecular2, decalDiffuse2.a);" << std::endl;
			}
		}else {
			ss << "	gBuffer1.a = 1.0;" << std::endl;
		}
		ss << "	gBuffer1.a += max(10.0, 10.0*floor(shininess/10.0));" << std::endl;		
		if (permutation.has(GBP_EMISSIVE_MAP)) {
			ss << "	gBuffer2.rgb = tex2D(emissiveMap, uv0).rgb;" << std::endl;
		}else {
			ss << "	gBuffer2.rgb = float3(0.0, 0.0, 0.0);" << std::endl;
		}
		ss << "	gBuffer2.a = custom.x;" << std::endl;
		ss << "}" << std::endl;

		Ogre::String programSource = ss.str();
		Ogre::String programName = "FP_" + Ogre::StringConverter::toString(permutation);

		Ogre::HighLevelGpuProgramPtr program = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "cg",Ogre:: GPT_FRAGMENT_PROGRAM);
		program->setSource(programSource);
		program->setParameter("entry_point", "toGBuffer_fp");
		program->setParameter("profiles", "ps_3_0");
		const Ogre::GpuProgramParametersSharedPtr& params = program->getDefaultParameters();		
		params->setNamedAutoConstant("diffuseColor", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
		params->setNamedAutoConstant("farDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
		params->setNamedAutoConstant("shininess", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
		params->setNamedAutoConstant("custom", Ogre::GpuProgramParameters::ACT_CUSTOM);
		program->load();
		return Ogre::GpuProgramPtr(program);
	}

	MaterialPtr TerrorMaterialGeneratorImpl::generateTemplateMaterial(Permutation permutation) {
		Ogre::String matName = "Material_" + Ogre::StringConverter::toString(permutation);
		MaterialPtr material = Ogre::MaterialManager::getSingleton().create(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Pass* pass = material->getTechnique(0)->getPass(0);
		pass->setName("Pass_" + Ogre::StringConverter::toString(permutation));
		pass->setLightingEnabled(false);
		pass->createTextureUnitState();
		if (permutation.has(GBP_NORMAL_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_SPECULAR_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_EMISSIVE_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_NORMAL_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_SPECULAR_MAP)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_DIFFUSE_MAP_2)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_NORMAL_MAP_2)) {
			pass->createTextureUnitState();
		}
		if (permutation.has(GBP_DECAL_SPECULAR_MAP_2)) {
			pass->createTextureUnitState();
		}
		return material;
	}
}


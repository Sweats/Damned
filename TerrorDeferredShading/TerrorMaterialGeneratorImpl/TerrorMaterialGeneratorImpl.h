#ifndef TERROR_DEFERRED_SHADING_TERROR_MATERIAL_GENERATOR_IMPL_H
#define TERROR_DEFERRED_SHADING_TERROR_MATERIAL_GENERATOR_IMPL_H

#include "TerrorPassPropertiesFactory/TerrorPassPropertiesFactory.h"

namespace terror_deferred_shading {
	class TerrorMaterialGeneratorImpl : public MaterialGeneratorImpl {
	public:
		TerrorMaterialGeneratorImpl();	

		virtual ~TerrorMaterialGeneratorImpl();

	protected:
		virtual Permutation getVertexShaderMask();
		virtual Permutation getFragmentShaderMask();
		virtual Permutation getMaterialMask();
		virtual Ogre::GpuProgramPtr generateVertexShader(Permutation permutation);
		virtual Ogre::GpuProgramPtr generateFragmentShader(Permutation permutation);
		virtual MaterialPtr generateTemplateMaterial(Permutation permutation);
	};

	typedef SharedPointer<TerrorMaterialGeneratorImpl> TerrorMaterialGeneratorImplPtr;
}

#endif

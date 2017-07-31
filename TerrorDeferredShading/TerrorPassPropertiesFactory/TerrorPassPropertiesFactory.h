#ifndef TERROR_DEFERRED_SHADING_TERROR_PASS_PROPERTIES_FACTORY_H
#define TERROR_DEFERRED_SHADING_TERROR_PASS_PROPERTIES_FACTORY_H

#include "TerrorPassProperties/TerrorPassProperties.h"

namespace terror_deferred_shading {
	class TerrorPassPropertiesFactory : public PassPropertiesFactory {
	public:
		TerrorPassPropertiesFactory();		

		virtual ~TerrorPassPropertiesFactory();

	private:
		PassPropertiesPtr create(Pass* pass);
	};

	typedef SharedPointer<TerrorPassPropertiesFactory> TerrorPassPropertiesFactoryPtr;
}

#endif

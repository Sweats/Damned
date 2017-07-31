#include "TerrorPassPropertiesFactory.h"

namespace terror_deferred_shading {
	TerrorPassPropertiesFactory::TerrorPassPropertiesFactory() {}		

	TerrorPassPropertiesFactory::~TerrorPassPropertiesFactory() {}

	PassPropertiesPtr TerrorPassPropertiesFactory::create(Pass* pass) {
		PassPropertiesPtr passProperties(new TerrorPassProperties);
		passProperties->initialize(pass);
		return passProperties;
	}
}

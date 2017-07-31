#ifndef BERSERKER_GHOST_VIEW_LISTENER_H
#define BERSERKER_GHOST_VIEW_LISTENER_H

#include "Config.h"

namespace terror {
	class BerserkerGhostViewListener : public PostProcessingEffect::Listener {
	public:
		BerserkerGhostViewListener() 
			: energy(0.0f) {}

		void setEnergy(float energy) {
			this->energy = energy;
		}

		void setEnergyRegenerationRate(float energyRegenerationRate) {
			this->energyRegenerationRate = energyRegenerationRate;
		}

		virtual ~BerserkerGhostViewListener() {}

	protected:
		float energy;
		float energyRegenerationRate;

	private:
		virtual void notifyMaterialRender(unsigned int passId, MaterialPtr& material) {		
			Ogre::GpuProgramParametersSharedPtr fragmentProgramParams = material->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fragmentProgramParams->setNamedConstant("energy", this->energy);
			fragmentProgramParams->setNamedConstant("energyRegenerationRate", this->energyRegenerationRate);
		}
	};

	typedef SharedPointer<BerserkerGhostViewListener> BerserkerGhostViewListenerPtr;
}

#endif

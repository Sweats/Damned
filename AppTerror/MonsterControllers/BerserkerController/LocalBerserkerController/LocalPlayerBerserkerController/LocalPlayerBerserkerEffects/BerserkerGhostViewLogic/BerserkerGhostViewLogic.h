#ifndef BERSERKER_GHOST_VIEW_LOGIC_H
#define BERSERKER_GHOST_VIEW_LOGIC_H

#include "BerserkerGhostViewListener/BerserkerGhostViewListener.h"

namespace terror {
	class BerserkerGhostViewLogic : public PostProcessingLogic {
	public:
		BerserkerGhostViewLogic() {}

		void setEnergy(float energy) {
			if (this->berserkerGhostViewListener) {
				this->berserkerGhostViewListener->setEnergy(energy);
			}
		}

		void setEnergyRegenerationRate(float energyRegenerationRate) {
			if (this->berserkerGhostViewListener) {
				this->berserkerGhostViewListener->setEnergyRegenerationRate(energyRegenerationRate);
			}
		}

		virtual ~BerserkerGhostViewLogic() {}

	protected:
		BerserkerGhostViewListenerPtr berserkerGhostViewListener;

	private:
		virtual void compositorInstanceCreated(PostProcessingEffect* instance) {
			if (!this->berserkerGhostViewListener) {
				this->berserkerGhostViewListener = BerserkerGhostViewListenerPtr(new BerserkerGhostViewListener);
			}
			instance->addListener(this->berserkerGhostViewListener.get());
		}
		virtual void compositorInstanceDestroyed(PostProcessingEffect* instance) {}
	};

	typedef SharedPointer<BerserkerGhostViewLogic> BerserkerGhostViewLogicPtr;
}

#endif

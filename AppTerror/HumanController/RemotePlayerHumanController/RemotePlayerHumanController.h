#ifndef TERROR_REMOTE_PLAYER_HUMAN_CONTROLLER_H
#define TERROR_REMOTE_PLAYER_HUMAN_CONTROLLER_H

#include "HumanController/HumanController.h"

namespace terror {
	class RemotePlayerHumanController : public HumanController, public EventHandler {
	public:
		RemotePlayerHumanController(HumanPtr human) : HumanController(human), EventHandler(60) {}

		virtual HumanControllerType getType() {
			return REMOTE_HUMAN_CONTROLLER;
		}

		virtual void update(HumanInfoPtr humanInfo) {
			if (this->human->isDead()) {
				return;
			}
			this->human->updateBasedOnInfo(humanInfo);
			this->human->setFlashlightState(humanInfo->flashlightState);
			this->human->setEyeDirection(humanInfo->getEyeDirection());
			this->human->setFlashlightBatteryCharge(humanInfo->flashlightBatteryCharge);
			this->human->setFlashlightIntensityFactor(humanInfo->flashlightIntensityFactor);
		}

		virtual void destroy(bool destroyHuman = true) {
			if (destroyHuman) {
				this->human->destroy();
			}
		}

		virtual ~RemotePlayerHumanController() {}

	private:
		virtual bool handleEvent(float diffTime) {
			this->human->updateBones(diffTime);
			return true;
		}
	};

	typedef SharedPointer<RemotePlayerHumanController> RemotePlayerHumanControllerPtr;
}

#endif
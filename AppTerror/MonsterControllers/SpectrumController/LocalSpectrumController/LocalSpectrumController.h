#ifndef TERROR_LOCAL_SPECTRUM_CONTROLLER_H
#define TERROR_LOCAL_SPECTRUM_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class LocalSpectrumController : public MonsterController {
	public:
		LocalSpectrumController(SpectrumPtr monster) : MonsterController(monster), spectrum(monster), monsterActionRequest(new MonsterActionRequest) {}

		MonsterActionRequestPtr getMonsterActionRequest() {
			ActorPtr actor = this->spectrum->getActor();
			this->monsterActionRequest->stateInfo->setTransform(actor->getPosition(), actor->getOrientation());
			this->monsterActionRequest->stateInfo->setVisible(this->spectrum->isVisible());
			this->monsterActionRequest->stateInfo->setCrouched(this->spectrum->isCrouched());

			return this->monsterActionRequest;
		}

		virtual ~LocalSpectrumController() {}

	protected:
		SpectrumPtr spectrum;
		MonsterActionRequestPtr monsterActionRequest;
	};

	typedef SharedPointer<LocalSpectrumController> LocalSpectrumControllerPtr;
}

#endif
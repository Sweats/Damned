#ifndef TERROR_LOCAL_BLIND_CONTROLLER_H
#define TERROR_LOCAL_BLIND_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class LocalBlindController : public MonsterController {
	public:
		LocalBlindController(BlindPtr monster) : MonsterController(monster), blind(monster), monsterActionRequest(new MonsterActionRequest) {}

		MonsterActionRequestPtr getMonsterActionRequest() {
			ActorPtr actor = this->blind->getActor();
			this->monsterActionRequest->stateInfo->setTransform(actor->getPosition(), actor->getOrientation());
			this->monsterActionRequest->stateInfo->setCrouched(this->blind->isCrouched());

			return this->monsterActionRequest;
		}

		virtual ~LocalBlindController() {}

	protected:
		BlindPtr blind;
		MonsterActionRequestPtr monsterActionRequest;
	};

	typedef SharedPointer<LocalBlindController> LocalBlindControllerPtr;
}

#endif
#ifndef TERROR_LOCAL_BERSERKER_CONTROLLER_H
#define TERROR_LOCAL_BERSERKER_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class LocalBerserkerController : public MonsterController {
	public:
		LocalBerserkerController(BerserkerPtr monster) : MonsterController(monster), berserker(monster), monsterActionRequest(new MonsterActionRequest) {}

		MonsterActionRequestPtr getMonsterActionRequest() {
			ActorPtr actor = this->berserker->getActor();
			this->monsterActionRequest->stateInfo->setTransform(actor->getPosition(), actor->getOrientation());
			this->monsterActionRequest->stateInfo->setVisible(this->berserker->isInBerserkerMode());
			this->monsterActionRequest->stateInfo->setCrouched(this->berserker->isCrouched());

			return this->monsterActionRequest;
		}

		virtual ~LocalBerserkerController() {}

	protected:
		BerserkerPtr berserker;
		MonsterActionRequestPtr monsterActionRequest;
	};

	typedef SharedPointer<LocalBerserkerController> LocalBerserkerControllerPtr;
}

#endif
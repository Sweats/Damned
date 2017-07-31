#ifndef TERROR_LOCAL_STATUE_CONTROLLER_H
#define TERROR_LOCAL_STATUE_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class LocalStatueController : public MonsterController {
	public:
		LocalStatueController(StatuePtr monster) : MonsterController(monster), statue(monster), monsterActionRequest(new MonsterActionRequest) {}

		MonsterActionRequestPtr getMonsterActionRequest() {
			ActorPtr actor = this->statue->getActor();
			this->monsterActionRequest->stateInfo->setTransform(actor->getPosition(), actor->getOrientation());
			this->monsterActionRequest->stateInfo->setState(int(this->statue->getState()));
			this->monsterActionRequest->stateInfo->setCrouched(this->statue->isCrouched());
			return this->monsterActionRequest;
		}

		virtual ~LocalStatueController() {}

	protected:
		StatuePtr statue;
		MonsterActionRequestPtr monsterActionRequest;
	};

	typedef SharedPointer<LocalStatueController> LocalStatueControllerPtr;
}

#endif
#ifndef TERROR_REMOTE_STATUE_CONTROLLER_H
#define TERROR_REMOTE_STATUE_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class RemotePlayerStatueController : public MonsterController {
	public:
		RemotePlayerStatueController(StatuePtr monster) : MonsterController(monster), statue(monster) {}

		virtual void update(MonsterInfoPtr monsterInfo) {
			this->statue->updateStatueInfo(monsterInfo);
		}

		virtual void destroy() {
			this->monster->destroy();
			static_ptr_cast<Statue>(this->monster)->cleanup();
		}

		virtual ~RemotePlayerStatueController() {}

	private:
		StatuePtr statue;

	private:
		virtual MonsterControllerType getType() {
			return REMOTE_MONSTER_CONTROLLER;
		}
	};

	typedef SharedPointer<RemotePlayerStatueController> RemotePlayerStatueControllerPtr;
}

#endif
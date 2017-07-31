#ifndef TERROR_REMOTE_BERSERKER_CONTROLLER_H
#define TERROR_REMOTE_BERSERKER_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class RemotePlayerBerserkerController : public MonsterController {
	public:
		RemotePlayerBerserkerController(BerserkerPtr monster) : MonsterController(monster) {}

		virtual void update(MonsterInfoPtr monsterInfo) {
			this->monster->updateInfo(monsterInfo);
		}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~RemotePlayerBerserkerController() {}

	private:
		virtual MonsterControllerType getType() {
			return REMOTE_MONSTER_CONTROLLER;
		}
	};

	typedef SharedPointer<RemotePlayerBerserkerController> RemotePlayerBerserkerControllerPtr;
}

#endif
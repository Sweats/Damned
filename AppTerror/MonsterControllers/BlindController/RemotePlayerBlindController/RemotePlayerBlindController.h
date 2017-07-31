#ifndef TERROR_REMOTE_BLIND_CONTROLLER_H
#define TERROR_REMOTE_BLIND_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class RemotePlayerBlindController : public MonsterController {
	public:
		RemotePlayerBlindController(BlindPtr monster) : MonsterController(monster) {}

		virtual void update(MonsterInfoPtr monsterInfo) {
			this->monster->updateInfo(monsterInfo);
		}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~RemotePlayerBlindController() {}

	private:
		virtual MonsterControllerType getType() {
			return REMOTE_MONSTER_CONTROLLER;
		}
	};

	typedef SharedPointer<RemotePlayerBlindController> RemotePlayerBlindControllerPtr;
}

#endif
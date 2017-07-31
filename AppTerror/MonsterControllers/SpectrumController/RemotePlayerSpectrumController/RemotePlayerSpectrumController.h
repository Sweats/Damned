#ifndef TERROR_REMOTE_SPECTRUM_CONTROLLER_H
#define TERROR_REMOTE_SPECTRUM_CONTROLLER_H

#include "MonsterControllers/MonsterController.h"

namespace terror {
	class RemotePlayerSpectrumController : public MonsterController {
	public:
		RemotePlayerSpectrumController(SpectrumPtr monster) : MonsterController(monster) {}

		virtual void update(MonsterInfoPtr monsterInfo) {
			this->monster->updateInfo(monsterInfo);
		}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~RemotePlayerSpectrumController() {}

	private:
		virtual MonsterControllerType getType() {
			return REMOTE_MONSTER_CONTROLLER;
		}
	};

	typedef SharedPointer<RemotePlayerSpectrumController> RemotePlayerSpectrumControllerPtr;
}

#endif
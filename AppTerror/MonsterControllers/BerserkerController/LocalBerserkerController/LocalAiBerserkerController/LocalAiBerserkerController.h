#ifndef TERROR_LOCAL_AI_BERSERKER_CONTROLLER_H
#define TERROR_LOCAL_AI_BERSERKER_CONTROLLER_H

#include "MonsterControllers/BerserkerController/LocalBerserkerController/LocalBerserkerController.h"

namespace terror {
	class LocalAiBerserkerController : public LocalBerserkerController, public EventHandler {
	public:
		LocalAiBerserkerController(BerserkerPtr monster) 
			: LocalBerserkerController(monster), EventHandler(60), time(0.0f) {}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~LocalAiBerserkerController() {}

	protected:
		float time;

	private:
		virtual MonsterControllerType getType() {
			return AI_MONSTER_CONTROLLER;
		}

		virtual bool handleEvent(float diffTime) {
			return true;
		}
	};

	typedef SharedPointer<LocalAiBerserkerController> LocalAiBerserkerControllerPtr;
}

#endif
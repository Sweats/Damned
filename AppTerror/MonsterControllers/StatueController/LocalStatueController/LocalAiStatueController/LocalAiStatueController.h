#ifndef TERROR_LOCAL_AI_STATUE_CONTROLLER_H
#define TERROR_LOCAL_AI_STATUE_CONTROLLER_H

#include "MonsterControllers/StatueController/LocalStatueController/LocalStatueController.h"

namespace terror {
	class LocalAiStatueController : public LocalStatueController, public EventHandler {
	public:
		LocalAiStatueController(StatuePtr monster) 
			: LocalStatueController(monster), EventHandler(60), time(0.0f) {}

		virtual void destroy() {
			this->monster->destroy();
			static_ptr_cast<Statue>(this->monster)->cleanup();
		}

		virtual ~LocalAiStatueController() {}

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

	typedef SharedPointer<LocalAiStatueController> LocalAiStatueControllerPtr;
}

#endif
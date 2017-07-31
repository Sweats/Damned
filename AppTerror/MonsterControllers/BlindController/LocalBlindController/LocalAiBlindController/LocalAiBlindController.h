#ifndef TERROR_LOCAL_AI_BLIND_CONTROLLER_H
#define TERROR_LOCAL_AI_BLIND_CONTROLLER_H

#include "MonsterControllers/BlindController/LocalBlindController/LocalBlindController.h"

namespace terror {
	class LocalAiBlindController : public LocalBlindController, public EventHandler {
	public:
		LocalAiBlindController(BlindPtr monster) 
			: LocalBlindController(monster), EventHandler(60), time(0.0f) {}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~LocalAiBlindController() {}

	protected:
		float time;

	private:
		virtual MonsterControllerType getType() {
			return AI_MONSTER_CONTROLLER;
		}

		virtual bool handleEvent(float diffTime) {
			//this->monster->update(diffTime);
			return true;
		}
	};

	typedef SharedPointer<LocalAiBlindController> LocalAiBlindControllerPtr;
}

#endif
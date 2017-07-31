#ifndef TERROR_LOCAL_AI_SPECTRUM_CONTROLLER_H
#define TERROR_LOCAL_AI_SPECTRUM_CONTROLLER_H

#include "MonsterControllers/SpectrumController/LocalSpectrumController/LocalSpectrumController.h"

namespace terror {
	class LocalAiSpectrumController : public LocalSpectrumController, public EventHandler {
	public:
		LocalAiSpectrumController(SpectrumPtr monster) 
			: LocalSpectrumController(monster), EventHandler(60), time(0.0f) {}

		virtual void destroy() {
			this->monster->destroy();
		}

		virtual ~LocalAiSpectrumController() {}

	protected:
		float time;

	private:
		virtual MonsterControllerType getType() {
			return AI_MONSTER_CONTROLLER;
		}

		virtual bool handleEvent(float diffTime) {
			this->spectrum->update(diffTime);
			return true;
		}
	};

	typedef SharedPointer<LocalAiSpectrumController> LocalAiSpectrumControllerPtr;
}

#endif
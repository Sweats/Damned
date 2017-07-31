#ifndef TERROR_PAUSE_HANDLER_H
#define TERROR_PAUSE_HANDLER_H

#include "Config.h"

namespace terror {
	class PauseHandler {
	public:
		PauseHandler() : paused(false), pausable(false) 
		{
			this->pause();
		}

		bool isPaused() const {
			return this->paused;
		}

		bool isPausable() const {
			return this->pausable;
		}

		void pause(bool forcePause = false) {
			if (!forcePause && !this->pausable) {
				return;
			}
			if (Core::getInstance()->getCurrentStateName() == "Ingame") {
				Core::getInstance()->switchState("Pause");
			}
			this->paused = true;
		}

		void resume() {
			if (!this->pausable) {
				return;
			}
			if (Core::getInstance()->getCurrentStateName() == "Pause" || Core::getInstance()->getCurrentStateName() == "Pregame") {
				Core::getInstance()->switchState("Ingame");
			}
			this->paused = false;
		}

		void setPausable(bool pausable) {
			this->pausable = pausable;
		}

		virtual ~PauseHandler() {}

	protected:
		bool paused;
		bool pausable;
	};
	
	typedef SharedPointer<PauseHandler> PauseHandlerPtr;
}

#endif
#ifndef TERROR_EXIT_TRIGGER_H
#define TERROR_EXIT_TRIGGER_H

#include "Config.h"

namespace terror {
	class ExitTrigger : public EventHandler {
	public:
		ExitTrigger() : EventHandler(20), exitRequested(false) {}

		void exit() {
			this->exitRequested = true;
		}

		virtual ~ExitTrigger() {}

	protected:
		bool exitRequested;

	private:
		virtual bool handleEvent(float diffTime) {
			if (this->exitRequested) {
				return false;
			}
			return true;
		}
	};

	typedef SharedPointer<ExitTrigger> ExitTriggerPtr;
}

#endif
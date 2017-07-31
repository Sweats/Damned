#ifndef TERROR_EXIT_BUTTON_HANDLER_H
#define TERROR_EXIT_BUTTON_HANDLER_H

#include "ExitTrigger/ExitTrigger.h"

namespace terror {
	class ExitButtonHandler : public ButtonHandler {
	public:
		ExitButtonHandler(Widget* quitButton, ExitTriggerPtr exitTrigger) 
			: ButtonHandler(quitButton), exitTrigger(exitTrigger) {}

		virtual ~ExitButtonHandler() {}

	protected:
		ExitTriggerPtr exitTrigger;

	private:
		virtual void handleButtonClick() {
			this->exitTrigger->exit();
		}	
	};

	typedef SharedPointer<ExitButtonHandler> ExitButtonHandlerPtr;
}

#endif
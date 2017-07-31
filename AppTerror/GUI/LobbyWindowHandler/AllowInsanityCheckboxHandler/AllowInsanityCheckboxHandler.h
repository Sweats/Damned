#ifndef TERROR_ALLOW_INSANITY_CHECKBOX_HANDLER_H
#define TERROR_ALLOW_INSANITY_CHECKBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class AllowInsanityCheckboxHandler : public CheckboxHandler {
	public:
		AllowInsanityCheckboxHandler(Widget* allowInsanityCheckbox, TerrorClientPtr terrorClient) 
			: CheckboxHandler(allowInsanityCheckbox), terrorClient(terrorClient) {}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setCheckboxState(gameSettings->isInsanityAllowed());
		}

		virtual ~AllowInsanityCheckboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCheckbox(bool state) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (state) {
				gameSettings->allowInsanity();
			}else {
				gameSettings->denyInsanity();
			}
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};
	typedef SharedPointer<AllowInsanityCheckboxHandler> AllowInsanityCheckboxHandlerPtr;
}

#endif
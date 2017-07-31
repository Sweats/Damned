#ifndef TERROR_ALLOW_SPECTATOR_CHECKBOX_HANDLER_H
#define TERROR_ALLOW_SPECTATOR_CHECKBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class AllowSpectatorCheckboxHandler : public CheckboxHandler {
	public:
		AllowSpectatorCheckboxHandler(Widget* allowSpectatorCheckbox, TerrorClientPtr terrorClient) 
			: CheckboxHandler(allowSpectatorCheckbox), terrorClient(terrorClient) {}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setCheckboxState(gameSettings->isSpectatorAllowed());
		}

		virtual ~AllowSpectatorCheckboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCheckbox(bool state) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (state) {
				gameSettings->allowSpectator();
			}else {
				gameSettings->denySpectator();
			}
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};
	typedef SharedPointer<AllowSpectatorCheckboxHandler> AllowSpectatorCheckboxHandlerPtr;
}

#endif
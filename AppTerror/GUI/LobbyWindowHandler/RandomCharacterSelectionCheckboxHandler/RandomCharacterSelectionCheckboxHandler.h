#ifndef TERROR_RANDOM_CHARACTER_SELECTION_CHECKBOX_HANDLER_H
#define TERROR_RANDOM_CHARACTER_SELECTION_CHECKBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class RandomCharacterSelectionCheckboxHandler : public CheckboxHandler {
	public:
		RandomCharacterSelectionCheckboxHandler(Widget* characterSelectionCheckbox, TerrorClientPtr terrorClient) 
			: CheckboxHandler(characterSelectionCheckbox), terrorClient(terrorClient) {}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setCheckboxState(gameSettings->getCharacterSelectionMode() == ALL_RANDOM_CHARACTERS);
		}

		virtual ~RandomCharacterSelectionCheckboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCheckbox(bool state) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (state) {
				gameSettings->setCharacterSelectionMode("All Random");
			}else {
				gameSettings->setCharacterSelectionMode("Selected");
			}
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};

	typedef SharedPointer<RandomCharacterSelectionCheckboxHandler> RandomCharacterSelectionCheckboxHandlerPtr;
}

#endif
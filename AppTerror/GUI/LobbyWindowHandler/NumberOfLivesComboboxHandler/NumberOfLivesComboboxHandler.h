#ifndef TERROR_NUMBER_OF_LIVES_COMBOBOX_HANDLER_H
#define TERROR_NUMBER_OF_LIVES_COMBOBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class NumberOfLivesComboboxHandler : public ComboboxHandler {
	public:
		NumberOfLivesComboboxHandler(Widget* numberOfLivesCombobox, TerrorClientPtr terrorClient) : ComboboxHandler(numberOfLivesCombobox), terrorClient(terrorClient) {
			for (int i = 1; i < 7; i++) {
				this->addComboboxItem(lexical_cast<string>(i));
			}
		}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setSelectedComboboxItem(lexical_cast<string>(gameSettings->getNumberOfLives()));
		}

		virtual ~NumberOfLivesComboboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCombobox(const string& selected) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->setNumberOfLives(selected);
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};
	typedef SharedPointer<NumberOfLivesComboboxHandler> NumberOfLivesComboboxHandlerPtr;
}

#endif
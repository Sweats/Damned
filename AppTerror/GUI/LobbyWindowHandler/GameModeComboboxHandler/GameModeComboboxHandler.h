#ifndef TERROR_GAME_MODE_COMBOBOX_HANDLER_H
#define TERROR_GAME_MODE_COMBOBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class GameModeComboboxHandler : public ComboboxHandler {
	public:
		GameModeComboboxHandler(Widget* gameModeCombobox, TerrorClientPtr terrorClient) : ComboboxHandler(gameModeCombobox), terrorClient(terrorClient) {
			this->addComboboxItem(toGameModeText(NORMAL_MODE));
			this->addComboboxItem(toGameModeText(LIFE_MODE));
		}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setSelectedComboboxItem(toGameModeText(gameSettings->getGameModeType()));
		}

		virtual ~GameModeComboboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCombobox(const string& selected) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->setGameMode(selected);
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};
	typedef SharedPointer<GameModeComboboxHandler> GameModeComboboxHandlerPtr;
}

#endif
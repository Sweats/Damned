#ifndef TERROR_ALLOWED_MONSTERS_WINDOW_HANDLER_H
#define TERROR_ALLOWED_MONSTERS_WINDOW_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"
#include "GUI/AllowedMonstersWindowHandler/AllowedMonstersHandler/AllowedMonstersHandler.h"

namespace terror {
	class AllowedMonstersWindowHandler {
	public:
		AllowedMonstersWindowHandler(GuiLayoutPtr allowedMonstersLayout, TerrorClientPtr terrorClient)
			: allowedMonstersLayout(allowedMonstersLayout), terrorClient(terrorClient) 
		{
			allowedMonstersLayout->connectShowEvent(Bind(&AllowedMonstersWindowHandler::updateLayout, this));

			this->allowedMonstersHandler = 
				AllowedMonstersHandlerPtr(new AllowedMonstersHandler(allowedMonstersLayout));

			Widget* okButton = allowedMonstersLayout->getWidget("OkButton");
			okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &AllowedMonstersWindowHandler::handleOkButton);
		}

		void reset() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->reset();
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}

		virtual ~AllowedMonstersWindowHandler() {}

	protected:
		GuiLayoutPtr allowedMonstersLayout;
		AllowedMonstersHandlerPtr allowedMonstersHandler;
		TerrorClientPtr terrorClient;

	private:
		void updateLayout() {
			this->allowedMonstersHandler->updateBasedOnSettings();
		}

		void handleOkButton(Widget* button) {
			if (!this->allowedMonstersHandler->setAllowedMonstersOnGameSettings()) {
				writeMessage("Please add at least one monster to the Allowed Monsters list");
				return;
			}
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
			this->allowedMonstersLayout->hide();
		}
	};

	typedef SharedPointer<AllowedMonstersWindowHandler> AllowedMonstersWindowHandlerPtr;
}

#endif
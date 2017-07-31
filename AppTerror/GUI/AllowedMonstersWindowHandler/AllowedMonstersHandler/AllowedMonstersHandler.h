#ifndef TERROR_ALLOWED_MONSTERS_HANDLER_H
#define TERROR_ALLOWED_MONSTERS_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class AllowedMonstersHandler {
	public:
		AllowedMonstersHandler(GuiLayoutPtr allowedMonstersLayout) {
			Widget* addMonsterButton = allowedMonstersLayout->getWidget("AddMonsterButton");
			addMonsterButton->eventMouseButtonClick += MyGUI::newDelegate(this, &AllowedMonstersHandler::handleAddMonsterButton);

			Widget* removeMonsterButton = allowedMonstersLayout->getWidget("RemoveMonsterButton");
			removeMonsterButton->eventMouseButtonClick += MyGUI::newDelegate(this, &AllowedMonstersHandler::handleRemoveMonsterButton);

			this->monstersListbox = static_cast<MyGUI::ListBox*>(allowedMonstersLayout->getWidget("MonstersListbox"));
			this->allowedMonstersListbox = static_cast<MyGUI::ListBox*>(allowedMonstersLayout->getWidget("AllowedMonstersListbox"));

			for (int i = 0; i < N_MONSTERS; ++i) {
				this->monstersListbox->addItem(toMonsterTypeText(i));
			}
		}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			
			this->allowedMonstersListbox->removeAllItems();
			for (int i = 0; i < N_MONSTERS; ++i) {
				if (gameSettings->getMonsters()[i]) {
					this->allowedMonstersListbox->addItem(toMonsterTypeText(i));
				}
			}
		}

		bool setAllowedMonstersOnGameSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();

			BoolVector& monsters = gameSettings->getMonsters();

			for (int i = 0; i < N_MONSTERS; ++i) {
				monsters[i] = false;
			}

			bool success = false;
			for (int i = 0; i < this->allowedMonstersListbox->getItemCount(); ++i) {
				monsters[toMonsterType(this->allowedMonstersListbox->getItemNameAt(i))] = true;
				success = true;
			}
			return success;
		}

		virtual ~AllowedMonstersHandler() {}

	protected:
		MyGUI::ListBox* monstersListbox;
		MyGUI::ListBox* allowedMonstersListbox;

	private:
		void handleAddMonsterButton(Widget* button) {
			if (this->monstersListbox->getIndexSelected() == MyGUI::ITEM_NONE) {
				return;
			}
			string monsterName = this->monstersListbox->getItemNameAt(this->monstersListbox->getIndexSelected());
			if (this->allowedMonstersListbox->findItemIndexWith(monsterName) != MyGUI::ITEM_NONE) {
				return;
			}
			this->allowedMonstersListbox->addItem(monsterName);
		}

		void handleRemoveMonsterButton(Widget* button) {
			if (this->allowedMonstersListbox->getIndexSelected() == MyGUI::ITEM_NONE) {
				return;
			}
			this->allowedMonstersListbox->removeItemAt(this->allowedMonstersListbox->getIndexSelected());
		}
	};

	typedef SharedPointer<AllowedMonstersHandler> AllowedMonstersHandlerPtr;
}

#endif
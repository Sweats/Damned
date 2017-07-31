#ifndef TERROR_STAGE_SELECTION_COMBOBOX_HANDLER_H
#define TERROR_STAGE_SELECTION_COMBOBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"
#include "GUI/StageSelectionWindowHandler/StageSelectionWindowHandler.h"

namespace terror {
	class StageSelectionComboboxHandler : public ComboboxHandler {
	public:
		StageSelectionComboboxHandler(Widget* stageSelectionCombobox, GuiLayoutPtr lobbyLayout, GuiLayoutPtr stageSelectionLayout, 
			StageSelectionWindowHandlerPtr stageSelectionWindowHandler, TerrorClientPtr terrorClient) 
			: ComboboxHandler(stageSelectionCombobox), stageSelectionCombobox(stageSelectionCombobox), lobbyLayout(lobbyLayout), stageSelectionLayout(stageSelectionLayout), 
			terrorClient(terrorClient), stageSelectionWindowHandler(stageSelectionWindowHandler) {
			this->addComboboxItem(toStageSelectionModeText(RANDOM));
			this->addComboboxItem(toStageSelectionModeText(CHOOSE_ONE));
		}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setSelectedComboboxItem(toStageSelectionModeText(gameSettings->getStageSelectionMode()));
		}

		virtual ~StageSelectionComboboxHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr stageSelectionLayout;
		TerrorClientPtr terrorClient;
		Widget* stageSelectionCombobox;
		StageSelectionWindowHandlerPtr stageSelectionWindowHandler;

	private:
		virtual void handleCombobox(const string& selected) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getStageSelectionMode() == CHOOSE_ONE && toStageSelectionMode(selected) == RANDOM) {
				this->setAllStageIds();
			}
			if (toStageSelectionMode(selected) == CHOOSE_ONE) {
				this->resetStageIds();
			}
			gameSettings->setStageSelectionMode(selected);
			this->stageSelectionWindowHandler->showWindow();
		}

		void setAllStageIds() {
			IntVector& stagesIds = GameSettings::getInstance()->getStagesIds();
			StageDatabasePtr stageDatabase = StageDatabase::getInstance();
			StringListPtr stagesNames = stageDatabase->getStagesNames();
			stagesIds.clear();
			foreach (std::string& stageName, *stagesNames) {
				stagesIds.push_back(stageDatabase->getStageId(stageName));
			}
		}

		void resetStageIds() {
			IntVector& stagesIds = GameSettings::getInstance()->getStagesIds();
			stagesIds.clear();
		}
	};

	typedef SharedPointer<StageSelectionComboboxHandler> StageSelectionComboboxHandlerPtr;	
}

#endif
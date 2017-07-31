#ifndef TERROR_STAGE_SELECTION_CHECKBOX_HANDLER_H
#define TERROR_STAGE_SELECTION_CHECKBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class StageCheckboxHandler : public CheckboxHandler {
	public:
		StageCheckboxHandler(GuiLayoutPtr lobbyLayout, GuiLayoutPtr stageSelectionLayout, Widget* stageCheckbox, int stageId, TerrorClientPtr terrorClient) 
			: CheckboxHandler(stageCheckbox), lobbyLayout(lobbyLayout), stageSelectionLayout(stageSelectionLayout), stageId(stageId), terrorClient(terrorClient) {}

		bool getCheckboxState() {
			return this->checkbox->getStateSelected();
		}

		int getStageId() {
			return this->stageId;
		}

		void updateBasedOnSettings() {
			this->setCheckboxState(false);
			foreach(int stageId, GameSettings::getInstance()->getStagesIds()) {
				if (this->stageId == stageId) {
					this->setCheckboxState(true);
				}
			}
		}	

		virtual ~StageCheckboxHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr stageSelectionLayout;
		TerrorClientPtr terrorClient;
		int stageId;

	private:
		virtual void handleCheckbox(bool state) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getStageSelectionMode() != CHOOSE_ONE) {
				return;
			}
			this->setStageIdOnStageIdsVector();
			this->broadcastGameSettings();
			this->stageSelectionLayout->hide();
		}

		void setStageIdOnStageIdsVector() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			IntVector& stageIds = gameSettings->getStagesIds();
			stageIds.clear();
			stageIds.push_back(this->stageId);
		}

		void broadcastGameSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};

	typedef SharedPointer<StageCheckboxHandler> StageCheckboxHandlerPtr;
}

#endif
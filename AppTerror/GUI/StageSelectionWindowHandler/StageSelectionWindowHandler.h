#ifndef TERROR_STAGE_SELECTION_WINDOW_HANDLER_H
#define TERROR_STAGE_SELECTION_WINDOW_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"
#include "GUI/StageSelectionWindowHandler/StageCheckboxHandler/StageCheckboxHandler.h"

namespace terror {
	typedef std::list<StageCheckboxHandlerPtr> StageCheckboxHandlerPtrList;

	class StageSelectionWindowHandler : public EventHandler {
	public:
		StageSelectionWindowHandler(GuiLayoutPtr lobbyLayout, GuiLayoutPtr stageSelectionLayout, TerrorClientPtr terrorClient) 
			: EventHandler(60), lobbyLayout(lobbyLayout), stageSelectionLayout(stageSelectionLayout), terrorClient(terrorClient), windowNeedsToBeShown(false) 
		{
			stageSelectionLayout->connectShowEvent(Bind(&StageSelectionWindowHandler::updateLayout, this));
			
			StageDatabasePtr stageDatabase = StageDatabase::getInstance();
			StringListPtr stagesNames = stageDatabase->getStagesNames();
			foreach(string stageName, *stagesNames) {
				try {
					int stageId = stageDatabase->getStageId(stageName);
					std::string stageCheckboxName = std::string("Stage") + lexical_cast<string>(stageId) + std::string("Checkbox");
					MyGUI::Button* stageCheckbox = static_cast<MyGUI::Button*>(stageSelectionLayout->getWidget(stageCheckboxName));
					this->stageCheckboxHandlers.push_back(StageCheckboxHandlerPtr(new StageCheckboxHandler(lobbyLayout, stageSelectionLayout, stageCheckbox, stageId, terrorClient)));
				}catch (std::exception& e) {}
			}

			this->okButton = stageSelectionLayout->getWidget("OKButton");
			this->okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &StageSelectionWindowHandler::handleOkButton);
			this->titleTextbox = static_cast<MyGUI::TextBox*>(stageSelectionLayout->getWidget("TitleTextbox"));
		}

		void showWindow() {
			this->windowNeedsToBeShown = true;
		}

		virtual ~StageSelectionWindowHandler() {}

	protected:
		GuiLayoutPtr lobbyLayout;
		GuiLayoutPtr stageSelectionLayout;
		StageCheckboxHandlerPtrList stageCheckboxHandlers;
		TerrorClientPtr terrorClient;
		Widget* okButton;
		MyGUI::TextBox* titleTextbox;
		bool windowNeedsToBeShown;

	private:
		virtual bool handleEvent(float diffTime) {
			if (this->windowNeedsToBeShown) {
				this->stageSelectionLayout->show(this->lobbyLayout, true);
				this->windowNeedsToBeShown = false;
			}
			return true;
		}

		void handleOkButton(Widget* button) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			IntVector& stagesIds = gameSettings->getStagesIds();
			stagesIds.clear();
			foreach (StageCheckboxHandlerPtr stageCheckboxHandler, this->stageCheckboxHandlers) {
				if (stageCheckboxHandler->getCheckboxState()) {
					stagesIds.push_back(stageCheckboxHandler->getStageId());
				}
			}
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
			this->stageSelectionLayout->hide();
		}

		void updateLayout() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			StageSelectionMode stageSelectionMode = gameSettings->getStageSelectionMode();
			if (stageSelectionMode == RANDOM) {
				this->treatRandomSelectionLayout();
			}else if (stageSelectionMode == CHOOSE_ONE) {
				treatChooseOneSelectionLayout();
			}else {
				throw runtime_error("Invalid stage selection mode.");
			}
		}

		void treatRandomSelectionLayout() {
			this->titleTextbox->setCaption("Check/uncheck the stages");
			this->updateCheckboxesBasedOnSettings();
			this->okButton->setVisible(true);
		}	

		void updateCheckboxesBasedOnSettings() {
			foreach (StageCheckboxHandlerPtr stageCheckboxHandler, this->stageCheckboxHandlers) {
				stageCheckboxHandler->updateBasedOnSettings();
			}
		}

		void treatChooseOneSelectionLayout() {
			this->titleTextbox->setCaption("Select a stage");
			foreach (StageCheckboxHandlerPtr stageCheckboxHandler, this->stageCheckboxHandlers) {
				stageCheckboxHandler->setCheckboxState(true);
			}
			this->okButton->setVisible(false);
		}

	};

	typedef SharedPointer<StageSelectionWindowHandler> StageSelectionWindowHandlerPtr;
}

#endif
#ifndef TERROR_PAUSE_WINDOW_HANDLER_H
#define TERROR_PAUSE_WINDOW_HANDLER_H

#include "PauseHandler/PauseHandler.h"
#include "GUI/ExitButtonHandler/ExitButtonHandler.h"

namespace terror {
	class ResumeGameButtonHandler : public ButtonHandler {
	public:
		ResumeGameButtonHandler(Widget* resumeGameButton, GuiLayoutPtr pauseLayout, PauseHandlerPtr pauseHandler)
			: ButtonHandler(resumeGameButton), pauseLayout(pauseLayout), pauseHandler(pauseHandler) {}

		virtual ~ResumeGameButtonHandler() {}

	protected:
		GuiLayoutPtr pauseLayout;
		PauseHandlerPtr pauseHandler;
		
	private:
		virtual void handleButtonClick() {
			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			mouseCursor->hide();
			this->pauseLayout->hide();
			this->pauseHandler->resume();
		}
	};
	typedef SharedPointer<ResumeGameButtonHandler> ResumeGameButtonHandlerPtr;

	class BackToTitleButtonHandler : public ButtonHandler {
	public:
		BackToTitleButtonHandler(Widget* backToTitleButton, GuiLayoutPtr pauseLayout, GuiLayoutPtr mainLayout, GuiLayoutPtr loadingLayout, 
			StageHandlerPtr stageHandler, TerrorClientPtr terrorClient) 
			: ButtonHandler(backToTitleButton), pauseLayout(pauseLayout), mainLayout(mainLayout), loadingLayout(loadingLayout), 
			stageHandler(stageHandler), terrorClient(terrorClient) {}

		virtual ~BackToTitleButtonHandler() {}

	protected:
		GuiLayoutPtr pauseLayout;
		GuiLayoutPtr mainLayout;
		GuiLayoutPtr loadingLayout;
		StageHandlerPtr stageHandler;
		TerrorClientPtr terrorClient;
		
	private:
		virtual void handleButtonClick() {
			LeaveGameRequestPtr leaveGameRequest(new LeaveGameRequest);
			this->terrorClient->sendObject(leaveGameRequest);
			this->terrorClient->reset();
			this->pauseLayout->hide();
			MyGUI::ImageBox* loadingImagebox = static_cast<MyGUI::ImageBox*>(this->loadingLayout->getWidget("LoadingImagebox"));
			loadingImagebox->setImageTexture(std::string("loading.jpg"));
			this->loadingLayout->show(this->mainLayout);
			this->stageHandler->requestUnloadStage();
		}
	};
	typedef SharedPointer<BackToTitleButtonHandler> BackToTitleButtonHandlerPtr;

	class PauseWindowHandler : public InputHandler {
	public:
		PauseWindowHandler(GuiLayoutPtr pauseLayout, GuiLayoutPtr mainLayout, GuiLayoutPtr loadingLayout, GuiLayoutPtr optionsLayout, GuiLayoutPtr startGameLayout,
			PauseHandlerPtr pauseHandler, StageHandlerPtr stageHandler, ControlsPtr controls, ExitTriggerPtr exitTrigger, TerrorClientPtr terrorClient) 
			: pauseLayout(pauseLayout), optionsLayout(optionsLayout), startGameLayout(startGameLayout), pauseHandler(pauseHandler), controls(controls), ingame(false)
		{
			this->resumeGameButtonHandler = ResumeGameButtonHandlerPtr(
				new ResumeGameButtonHandler(this->pauseLayout->getWidget("ResumeGameButton"), this->pauseLayout, this->pauseHandler));
			this->optionsButtonHandler = WindowCallerButtonHandlerPtr(
				new WindowCallerButtonHandler(this->pauseLayout->getWidget("OptionsButton"), this->pauseLayout, this->optionsLayout));
			this->backToTitleButtonHandler = BackToTitleButtonHandlerPtr(
				new BackToTitleButtonHandler(this->pauseLayout->getWidget("BackToTitleButton"), pauseLayout, mainLayout, loadingLayout, stageHandler, terrorClient));
			this->exitButtonHandler = ExitButtonHandlerPtr(
				new ExitButtonHandler(this->pauseLayout->getWidget("ExitGameButton"), exitTrigger));			
		}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->ingame = false;
		}

		virtual ~PauseWindowHandler() {}

	protected:
		GuiLayoutPtr pauseLayout;
		GuiLayoutPtr optionsLayout;
		GuiLayoutPtr startGameLayout;
		ControlsPtr controls;
		ResumeGameButtonHandlerPtr resumeGameButtonHandler;
		WindowCallerButtonHandlerPtr optionsButtonHandler;		
		BackToTitleButtonHandlerPtr backToTitleButtonHandler;
		ExitButtonHandlerPtr exitButtonHandler;
		PauseHandlerPtr pauseHandler;
		bool ingame;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (!this->ingame || ((button != this->controls->pause) && (button != BC_ESCAPE))) {
				return;
			}
			MouseCursorPtr mouseCursor = MouseCursor::getInstance();
			if ((this->pauseHandler->isPausable() && !this->pauseHandler->isPaused()) || !mouseCursor->isVisible()) {
				mouseCursor->show();
				if (Core::getInstance()->getCurrentStateName() == "Pregame") {
					this->pauseLayout->show(this->startGameLayout, false);
				}else {
					this->pauseLayout->show(GuiLayoutPtr(), false);
				}
				this->pauseHandler->pause();
			}else if (this->pauseLayout->isVisible()) {
				mouseCursor->hide();
				this->pauseLayout->hide();
				if (this->pauseHandler->isPausable()) {
					this->pauseHandler->resume();
				}
			}
		}
	};

	typedef SharedPointer<PauseWindowHandler> PauseWindowHandlerPtr;
}

#endif
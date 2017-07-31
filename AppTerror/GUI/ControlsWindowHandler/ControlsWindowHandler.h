#ifndef TERROR_CONTROLS_WINDOW_HANDLER_H
#define TERROR_CONTROLS_WINDOW_HANDLER_H

#include "MouseSensibilitySliderHandler/MouseSensibilitySliderHandler.h"
#include "InvertMouseXCheckboxHandler/InvertMouseXCheckboxHandler.h"
#include "InvertMouseYCheckboxHandler/InvertMouseYCheckboxHandler.h"
#include "ControlButtonHandler/MoveForwardButtonHandler/MoveForwardButtonHandler.h"
#include "ControlButtonHandler/MoveBackButtonHandler/MoveBackButtonHandler.h"
#include "ControlButtonHandler/MoveLeftButtonHandler/MoveLeftButtonHandler.h"
#include "ControlButtonHandler/MoveRightButtonHandler/MoveRightButtonHandler.h"
#include "ControlButtonHandler/SlowWalkButtonHandler/SlowWalkButtonHandler.h"
#include "ControlButtonHandler/CrouchButtonHandler/CrouchButtonHandler.h"
#include "ControlButtonHandler/RunButtonHandler/RunButtonHandler.h"
#include "ControlButtonHandler/PushToTalkButtonHandler/PushToTalkButtonHandler.h"
#include "ControlButtonHandler/TransformButtonHandler/TransformButtonHandler.h"
#include "ControlButtonHandler/GrabButtonHandler/GrabButtonHandler.h"
#include "ControlButtonHandler/SwitchFlashlightButtonHandler/SwitchFlashlightButtonHandler.h"
#include "ControlButtonHandler/AttackButtonHandler/AttackButtonHandler.h"
#include "ControlButtonHandler/TeleportButtonHandler/TeleportButtonHandler.h"
#include "ControlButtonHandler/GuiAcceptButtonHandler/GuiAcceptButtonHandler.h"
#include "ControlButtonHandler/StartButtonHandler/StartButtonHandler.h"
#include "ControlButtonHandler/PauseButtonHandler/PauseButtonHandler.h"
#include "ControlButtonHandler/SpectateNextButtonHandler/SpectateNextButtonHandler.h"
#include "ControlButtonHandler/TabButtonHandler/TabButtonHandler.h"
#include "ControlButtonHandler/GuiReturnButtonHandler/GuiReturnButtonHandler.h"
#include "LoadDefaultControlPreferencesButtonHandler/LoadDefaultControlPreferencesButtonHandler.h"


namespace terror {
	class ControlsWindowHandler : public InputHandler {
	public:
		ControlsWindowHandler(GuiLayoutPtr controlsLayout, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator, GuiSystemPtr guiSystem) 
			: controlsLayout(controlsLayout), mouseCursor(MouseCursor::getInstance()), terrorSave(terrorSave), enableControlsLayout(false)
		{
			this->mouseSensibilitySliderHandler = 
				MouseSensibilitySliderHandlerPtr(new MouseSensibilitySliderHandler(controlsLayout->getWidget("MouseSensibilitySlider"), terrorSave));

			this->invertMouseXCheckboxHandler = 
				InvertMouseXCheckboxHandlerPtr(new InvertMouseXCheckboxHandler(controlsLayout->getWidget("InvertMouseXCheckbox"), terrorSave));

			this->invertMouseYCheckboxHandler = 
				InvertMouseYCheckboxHandlerPtr(new InvertMouseYCheckboxHandler(controlsLayout->getWidget("InvertMouseYCheckbox"), terrorSave));

			this->controlButtonHandlers = ControlButtonHandlerPtrListPtr(new ControlButtonHandlerPtrList);

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new MoveForwardButtonHandler(controlsLayout->getWidget("MoveForwardButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new MoveBackButtonHandler(controlsLayout->getWidget("MoveBackButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new MoveLeftButtonHandler(controlsLayout->getWidget("MoveLeftButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new MoveRightButtonHandler(controlsLayout->getWidget("MoveRightButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new SlowWalkButtonHandler(controlsLayout->getWidget("SlowWalkButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new CrouchButtonHandler(controlsLayout->getWidget("CrouchButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new RunButtonHandler(controlsLayout->getWidget("RunButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new PushToTalkButtonHandler(controlsLayout->getWidget("PushToTalkButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new TransformButtonHandler(controlsLayout->getWidget("TransformButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new GrabButtonHandler(controlsLayout->getWidget("GrabButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new SwitchFlashlightButtonHandler(controlsLayout->getWidget("SwitchFlashlightButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new AttackButtonHandler(controlsLayout->getWidget("AttackButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new TeleportButtonHandler(controlsLayout->getWidget("TeleportButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new GuiAcceptButtonHandler(controlsLayout->getWidget("GuiAcceptButton"), controlsLayout, terrorSave, buttonTranslator, guiSystem)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new StartButtonHandler(controlsLayout->getWidget("StartButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new PauseButtonHandler(controlsLayout->getWidget("PauseButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new SpectateNextButtonHandler(controlsLayout->getWidget("SpectateNextButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new TabButtonHandler(controlsLayout->getWidget("TabButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->controlButtonHandlers->add(
				ControlButtonHandlerPtr(new GuiReturnButtonHandler(controlsLayout->getWidget("GuiReturnButton"), controlsLayout, terrorSave, buttonTranslator)));

			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(controlsLayout->getWidget("BackButton"), controlsLayout));

			this->loadDefaultControlPreferencesButtonHandler = 
				LoadDefaultControlPreferencesButtonHandlerPtr(new LoadDefaultControlPreferencesButtonHandler(controlsLayout->getWidget("LoadDefaultControlPreferencesButton"),
				terrorSave,this->controlButtonHandlers, mouseSensibilitySliderHandler, invertMouseXCheckboxHandler, invertMouseYCheckboxHandler));
		}

		virtual ~ControlsWindowHandler() {}

	protected:
		MouseSensibilitySliderHandlerPtr mouseSensibilitySliderHandler;
		InvertMouseXCheckboxHandlerPtr invertMouseXCheckboxHandler;
		InvertMouseYCheckboxHandlerPtr invertMouseYCheckboxHandler;
		ControlButtonHandlerPtrListPtr controlButtonHandlers;
		BackButtonHandlerPtr backButtonHandler;
		LoadDefaultControlPreferencesButtonHandlerPtr loadDefaultControlPreferencesButtonHandler;
		GuiLayoutPtr controlsLayout;
		TerrorSavePtr terrorSave;
		MouseCursorPtr mouseCursor;
		bool enableControlsLayout;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			foreach(ControlButtonHandlerPtr controlButtonHandler, *this->controlButtonHandlers){
				if (controlButtonHandler->wasButtonPressed()) {
					controlButtonHandler->bindKeyToCommand(button);
					this->enableControlsLayout = true;
					return;
				}
			}
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->controlsLayout->isVisible()) {
				this->controlsLayout->hide();
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (this->enableControlsLayout) {
				this->controlsLayout->enable();
				this->mouseCursor->show();
				this->enableControlsLayout = false;
			}
		}
	};

	typedef SharedPointer<ControlsWindowHandler> ControlsWindowHandlerPtr;
}

#endif
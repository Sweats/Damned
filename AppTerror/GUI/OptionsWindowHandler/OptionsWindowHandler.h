#ifndef TERROR_OPTIONS_WINDOW_HANDLER_H
#define TERROR_OPTIONS_WINDOW_HANDLER_H

#include "TerrorSave/TerrorSave.h"


namespace terror {
	class OptionsWindowHandler : public InputHandler {
	public:
		OptionsWindowHandler(GuiLayoutPtr optionsLayout, GuiLayoutPtr graphicsLayout, GuiLayoutPtr soundLayout, GuiLayoutPtr controlsLayout,
			GuiLayoutPtr profileLayout, GuiLayoutPtr connectionLayout, TerrorSavePtr terrorSave) 
			: optionsLayout(optionsLayout), terrorSave(terrorSave)
		{
			this->graphicsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("GraphicsButton"), optionsLayout, graphicsLayout));
			this->soundButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("SoundButton"), optionsLayout, soundLayout));
			this->controlsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("ControlsButton"), optionsLayout, controlsLayout));
			this->profileButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("ProfileButton"), optionsLayout, profileLayout));
			this->connectionButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("ConnectionButton"), optionsLayout, connectionLayout));
			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(optionsLayout->getWidget("BackButton"), optionsLayout));
		}

		OptionsWindowHandler(GuiLayoutPtr optionsLayout, GuiLayoutPtr graphicsLayout, GuiLayoutPtr soundLayout, GuiLayoutPtr controlsLayout,
			GuiLayoutPtr connectionLayout, TerrorSavePtr terrorSave) 
			: optionsLayout(optionsLayout), terrorSave(terrorSave)
		{
			this->graphicsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("GraphicsButton"), optionsLayout, graphicsLayout));
			this->soundButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("SoundButton"), optionsLayout, soundLayout));
			this->controlsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("ControlsButton"), optionsLayout, controlsLayout));
			this->connectionButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(optionsLayout->getWidget("ConnectionButton"), optionsLayout, connectionLayout));
			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(optionsLayout->getWidget("BackButton"), optionsLayout));
		}

		virtual ~OptionsWindowHandler() {}

	protected:
		GuiLayoutPtr optionsLayout;
		TerrorSavePtr terrorSave;
		WindowCallerButtonHandlerPtr graphicsButtonHandler;
		WindowCallerButtonHandlerPtr soundButtonHandler;
		WindowCallerButtonHandlerPtr controlsButtonHandler;
		WindowCallerButtonHandlerPtr profileButtonHandler;	
		WindowCallerButtonHandlerPtr connectionButtonHandler;	
		BackButtonHandlerPtr backButtonHandler;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->optionsLayout->isVisible()) {
				this->optionsLayout->hide();
			}
		}
	};

	typedef SharedPointer<OptionsWindowHandler> OptionsWindowHandlerPtr;
}

#endif
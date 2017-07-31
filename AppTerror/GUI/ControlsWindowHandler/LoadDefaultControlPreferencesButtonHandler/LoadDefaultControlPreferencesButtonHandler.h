#ifndef TERROR_LOAD_DEFAULT_PREFERENCES_BUTTON_HANDLER_H
#define TERROR_LOAD_DEFAULT_PREFERENCES_BUTTON_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class LoadDefaultControlPreferencesButtonHandler : public ButtonHandler {
	public:
		LoadDefaultControlPreferencesButtonHandler(Widget* loadDefaultControlPreferencesButton, TerrorSavePtr terrorSave, ControlButtonHandlerPtrListPtr controlButtonHandlers,
			MouseSensibilitySliderHandlerPtr mouseSensibilitySliderHandler,	InvertMouseXCheckboxHandlerPtr invertMouseXCheckboxHandler,	
			InvertMouseYCheckboxHandlerPtr invertMouseYCheckboxHandler)
			: ButtonHandler(loadDefaultControlPreferencesButton), terrorSave(terrorSave), controlButtonHandlers(controlButtonHandlers), 
			mouseSensibilitySliderHandler(mouseSensibilitySliderHandler), invertMouseXCheckboxHandler(invertMouseXCheckboxHandler), invertMouseYCheckboxHandler(invertMouseYCheckboxHandler){}

		virtual ~LoadDefaultControlPreferencesButtonHandler() {}

	protected:
		ControlButtonHandlerPtrListPtr controlButtonHandlers;
		MouseSensibilitySliderHandlerPtr mouseSensibilitySliderHandler;
		InvertMouseXCheckboxHandlerPtr invertMouseXCheckboxHandler;
		InvertMouseYCheckboxHandlerPtr invertMouseYCheckboxHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleButtonClick() {
			this->terrorSave->controls->loadDefaults();
			this->mouseSensibilitySliderHandler->updateSliderPosition();
			this->invertMouseXCheckboxHandler->updateCheckboxState();
			this->invertMouseYCheckboxHandler->updateCheckboxState();
			foreach(ControlButtonHandlerPtr controlButtonHandler, *this->controlButtonHandlers)
				controlButtonHandler->updateButtonCaption();
			this->terrorSave->save();
		}
	};

	typedef SharedPointer<LoadDefaultControlPreferencesButtonHandler> LoadDefaultControlPreferencesButtonHandlerPtr;
}

#endif
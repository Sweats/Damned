#ifndef TERROR_MOUSE_SENSIBILITY_SLIDER_HANDLER_H
#define TERROR_MOUSE_SENSIBILITY_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class MouseSensibilitySliderHandler : public SliderHandler {
	public:
		MouseSensibilitySliderHandler(Widget* mouseSensibilitySlider, TerrorSavePtr terrorSave)
			: SliderHandler(mouseSensibilitySlider), terrorSave(terrorSave), mouseCursor(MouseCursor::getInstance()) 
		{
			this->setSliderPosition(static_cast<int>((this->terrorSave->controls->mouseSensibility - 0.15f)*20.0f/3.0f));
		}

		void updateSliderPosition() {
			this->setSliderPosition(static_cast<int>((this->terrorSave->controls->mouseSensibility - 0.15f)*20.0f/3.0f));
		}

		virtual ~MouseSensibilitySliderHandler() {}

	protected:
		TerrorSavePtr terrorSave;
		MouseCursorPtr mouseCursor;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->controls->mouseSensibility = 0.15f + static_cast<float>(position)*3.0f/20.0f;
			this->terrorSave->save();
		}
	};

	typedef SharedPointer<MouseSensibilitySliderHandler> MouseSensibilitySliderHandlerPtr;
}

#endif
#ifndef TERROR_FULLSCREEN_CHECKBOX_HANDLER_H
#define TERROR_FULLSCREEN_CHECKBOX_HANDLER_H

#include "Config.h"

namespace terror {
	class FullscreenCheckboxHandler : public CheckboxHandler {
	public:
		FullscreenCheckboxHandler(Widget* fullscreenCheckbox, GraphicsOptionsPtr graphicsOptions) 
			: CheckboxHandler(fullscreenCheckbox), graphicsOptions(graphicsOptions) 
		{
			std::string fullscreen = this->graphicsOptions->getCurrentValue(GraphicsOptions::FULLSCREEN);
			if (fullscreen == "Yes") {
				this->setCheckboxState(true);
			}else {
				this->setCheckboxState(false);
			}
		}

		virtual ~FullscreenCheckboxHandler() {}

	protected:
		GraphicsOptionsPtr graphicsOptions;

	private:
		virtual void handleCheckbox(bool state) {
			std::string fullscreen = "Yes";
			if (!state) {
				fullscreen = "No";
			}
			this->graphicsOptions->setFullscreen(fullscreen);
		}
	};

	typedef SharedPointer<FullscreenCheckboxHandler> FullscreenCheckboxHandlerPtr;
}

#endif
#ifndef TERROR_V_SYNC_CHECKBOX_HANDLER_H
#define TERROR_V_SYNC_CHECKBOX_HANDLER_H

#include "Config.h"

namespace terror {
	class VSyncCheckboxHandler : public CheckboxHandler {
	public:
		VSyncCheckboxHandler(Widget* vSyncCheckbox, GraphicsOptionsPtr graphicsOptions) 
			: CheckboxHandler(vSyncCheckbox), graphicsOptions(graphicsOptions) 
		{
			std::string vSync = this->graphicsOptions->getCurrentValue(GraphicsOptions::V_SYNC);
			if (vSync == "Yes") {
				this->checkbox->setStateSelected(true);
			}
		}

		virtual ~VSyncCheckboxHandler() {}

	protected:
		GraphicsOptionsPtr graphicsOptions;

	private:
		virtual void handleCheckbox(bool state) {
			std::string vSync = "Yes";
			if (!state) {
				vSync = "No";
			}
			this->graphicsOptions->setVSync(vSync);
		}
	};

	typedef SharedPointer<VSyncCheckboxHandler> VSyncCheckboxHandlerPtr;
}

#endif
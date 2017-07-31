#ifndef TERROR_RESOLUTIONS_COMBOBOX_HANDLER_H
#define TERROR_RESOLUTIONS_COMBOBOX_HANDLER_H

#include "Config.h"

namespace terror {
	class ResolutionsComboboxHandler : public ComboboxHandler {
	public:
		ResolutionsComboboxHandler(Widget* resolutionsCombobox, GraphicsOptionsPtr graphicsOptions) 
			: ComboboxHandler(resolutionsCombobox), graphicsOptions(graphicsOptions) 
		{
			StringListPtr resolutions = this->graphicsOptions->getPossibleResolutions();			
			foreach (std::string& resolution, *resolutions) {
				this->addComboboxItem(resolution);
			}
			this->setSelectedComboboxItem(this->graphicsOptions->getCurrentValue(GraphicsOptions::RESOLUTION));
		}

		virtual ~ResolutionsComboboxHandler() {}

	protected:
		GraphicsOptionsPtr graphicsOptions;

	private:
		virtual void handleCombobox(const string& selected) {
			this->graphicsOptions->setResolution(selected);
		}
	};

	typedef SharedPointer<ResolutionsComboboxHandler> ResolutionsComboboxHandlerPtr;
}

#endif
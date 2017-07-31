#ifndef TERROR_FXAA_COMBOBOX_HANDLER_H
#define TERROR_FXAA_COMBOBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class FxaaComboboxHandler : public ComboboxHandler {
	public:
		FxaaComboboxHandler(Widget* fxaaCombobox, DeferredShadingSystemPtr deferredShadingSystem, TerrorSavePtr terrorSave);

		virtual ~FxaaComboboxHandler();

	protected:
		DeferredShadingSystemPtr deferredShadingSystem;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCombobox(const string& selected);
		void setFxaa(int fxaaQuality);
	};

	typedef SharedPointer<FxaaComboboxHandler> FxaaComboboxHandlerPtr;
}

#endif
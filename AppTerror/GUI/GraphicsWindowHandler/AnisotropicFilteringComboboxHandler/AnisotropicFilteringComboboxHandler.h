#ifndef TERROR_ANISOTROPIC_FILTERING_COMBOBOX_HANDLER_H
#define TERROR_ANISOTROPIC_FILTERING_COMBOBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class AnisotropicFilteringComboboxHandler : public ComboboxHandler {
	public:
		AnisotropicFilteringComboboxHandler(Widget* anisotropicFilteringCombobox, TerrorSavePtr terrorSave);

		virtual ~AnisotropicFilteringComboboxHandler();

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCombobox(const string& selected);
	};

	typedef SharedPointer<AnisotropicFilteringComboboxHandler> AnisotropicFilteringComboboxHandlerPtr;
}

#endif
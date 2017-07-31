#ifndef TERROR_CREDITS_WINDOW_HANDLER_H
#define TERROR_CREDITS_WINDOW_HANDLER_H

#include "Config.h"

namespace terror {
	class CreditsWindowHandler : public InputHandler {
	public:
		CreditsWindowHandler(GuiLayoutPtr creditsLayout, GuiLayoutPtr mainLayout, TerrorSavePtr terrorSave)
			: creditsLayout(creditsLayout), terrorSave(terrorSave)
		{
			MyGUI::ImageBox* creditsImagebox = static_cast<MyGUI::ImageBox*>(this->creditsLayout->getWidget("CreditsImagebox"));
			creditsImagebox->setImageTexture(std::string("damnedcredits.jpg"));
			this->backButtonHandler = WindowCallerButtonHandlerPtr(
				new WindowCallerButtonHandler(this->creditsLayout->getWidget("BackButton"), this->creditsLayout, mainLayout));
		}

	protected:
		GuiLayoutPtr creditsLayout;
		TerrorSavePtr terrorSave;
		WindowCallerButtonHandlerPtr backButtonHandler;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->creditsLayout->isVisible()) {
				this->creditsLayout->hide();
			}
		}
	};

	typedef SharedPointer<CreditsWindowHandler> CreditsWindowHandlerPtr;
}

#endif
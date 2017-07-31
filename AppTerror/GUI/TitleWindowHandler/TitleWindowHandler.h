#ifndef TERROR_TITLE_WINDOW_HANDLER_H
#define TERROR_TITLE_WINDOW_HANDLER_H

#include "GUI/ExitButtonHandler/ExitButtonHandler.h"
#include "LogoffButtonHandler/LogoffButtonHandler.h"

namespace terror {
	class TitleWindowHandler {
	public:
		TitleWindowHandler(GuiLayoutPtr mainLayout, GuiLayoutPtr hostLayout, GuiLayoutPtr joinLayout, 
			GuiLayoutPtr optionsLayout, GuiLayoutPtr loginLayout, GuiLayoutPtr creditsLayout, TerrorClientPtr terrorClient, ExitTriggerPtr exitTrigger, 
			LoginSaverPtr loginSaver, PlayerProfilePtr playerProfile) 
		{
			this->hostButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(mainLayout->getWidget("HostGameButton"), mainLayout, hostLayout));
			this->joinButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(mainLayout->getWidget("JoinGameButton"), mainLayout, joinLayout));
			this->optionsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(mainLayout->getWidget("OptionsButton"), mainLayout, optionsLayout));
			this->creditsButtonHandler = 
				WindowCallerButtonHandlerPtr(new WindowCallerButtonHandler(mainLayout->getWidget("CreditsButton"), mainLayout, creditsLayout));
			this->logoffButtonHandler = 
				LogoffButtonHandlerPtr(new LogoffButtonHandler(mainLayout, loginLayout, loginSaver, playerProfile, terrorClient));
			this->exitButtonHandler = 
				ExitButtonHandlerPtr(new ExitButtonHandler(mainLayout->getWidget("ExitButton"), exitTrigger));			
		}

		virtual ~TitleWindowHandler() {}

	protected:
		WindowCallerButtonHandlerPtr hostButtonHandler;
		WindowCallerButtonHandlerPtr joinButtonHandler;
		WindowCallerButtonHandlerPtr optionsButtonHandler;
		WindowCallerButtonHandlerPtr creditsButtonHandler;
		LogoffButtonHandlerPtr logoffButtonHandler;
		ExitButtonHandlerPtr exitButtonHandler;		
	};

	typedef SharedPointer<TitleWindowHandler> TitleWindowHandlerPtr;
}

#endif
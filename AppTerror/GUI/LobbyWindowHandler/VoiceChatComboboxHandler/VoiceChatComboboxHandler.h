#ifndef TERROR_VOICE_CHAT_COMBOBOX_HANDLER_H
#define TERROR_VOICE_CHAT_COMBOBOX_HANDLER_H

#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	class VoiceChatComboboxHandler : public ComboboxHandler {
	public:
		VoiceChatComboboxHandler(Widget* voiceChatCombobox, TerrorClientPtr terrorClient) : ComboboxHandler(voiceChatCombobox), terrorClient(terrorClient) {
			this->addComboboxItem(toVoiceChatModeText(VOIP_3D_POSITIONAL));
			this->addComboboxItem(toVoiceChatModeText(VOIP_SIMPLE));
			this->addComboboxItem(toVoiceChatModeText(VOIP_DISABLED));
		}

		void updateBasedOnSettings() {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			this->setSelectedComboboxItem(toVoiceChatModeText(gameSettings->getVoiceChatMode()));
		}

		virtual ~VoiceChatComboboxHandler() {}

	protected:
		TerrorClientPtr terrorClient;

	private:
		virtual void handleCombobox(const string& selected) {
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			gameSettings->setVoiceChatMode(selected);
			gameSettings->notifyUpdate();
			this->terrorClient->sendObjectToPlayers(gameSettings, true);
		}
	};
	typedef SharedPointer<VoiceChatComboboxHandler> VoiceChatComboboxHandlerPtr;
}

#endif
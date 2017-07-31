#ifndef TERROR_CAPTURE_MODE_COMBOBOX_HANDLER_H
#define TERROR_CAPTURE_MODE_COMBOBOX_HANDLER_H

#include "TerrorSave/TerrorSave.h"
#include "VoiceChatHandler/VoiceChatHandler.h"

namespace terror {
	class CaptureModeComboboxHandler : public ComboboxHandler {
	public:
		CaptureModeComboboxHandler(Widget* captureModeCombobox, VoiceChatHandlerPtr voiceChatHandler, TerrorSavePtr terrorSave) 
			: ComboboxHandler(captureModeCombobox), voiceChatHandler(voiceChatHandler), terrorSave(terrorSave) 
		{
			this->addComboboxItem("Voice Activation");
			this->addComboboxItem("Push To Talk");

			this->reset(terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&CaptureModeComboboxHandler::reset, this, _1));
		}

		virtual ~CaptureModeComboboxHandler() {}

	protected:
		VoiceChatHandlerPtr voiceChatHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleCombobox(const string& selected) {
			terrorSave->useVoiceActivation = selected == "Voice Activation" ? true : false;
			terrorSave->save();

			CaptureMode captureMode = terrorSave->useVoiceActivation ? VOICE_ACTIVATION : PUSH_TO_TALK;
			this->voiceChatHandler->setCaptureMode(captureMode);
		}

		void reset(TerrorSave* terrorSave) {
			CaptureMode captureMode = terrorSave->useVoiceActivation ? VOICE_ACTIVATION : PUSH_TO_TALK;
			this->voiceChatHandler->setCaptureMode(captureMode);			

			string captureModeText = terrorSave->useVoiceActivation ? "Voice Activation" : "Push To Talk";
			this->setSelectedComboboxItem(captureModeText);
		}
	};

	typedef SharedPointer<CaptureModeComboboxHandler> CaptureModeComboboxHandlerPtr;
}

#endif
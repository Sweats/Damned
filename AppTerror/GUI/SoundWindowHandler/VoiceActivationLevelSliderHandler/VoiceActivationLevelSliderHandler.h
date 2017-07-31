#ifndef TERROR_VOICE_ACTIVATION_LEVEL_SLIDER_HANDLER_H
#define TERROR_VOICE_ACTIVATION_LEVEL_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"
#include "VoiceChatHandler/VoiceChatHandler.h"

namespace terror {
	class VoiceActivationLevelSliderHandler : public SliderHandler {
	public:
		VoiceActivationLevelSliderHandler(Widget* voiceActivationLevelSlider, VoiceChatHandlerPtr voiceChatHandler, TerrorSavePtr terrorSave)
			: SliderHandler(voiceActivationLevelSlider), voiceChatHandler(voiceChatHandler), terrorSave(terrorSave)
		{
			this->reset(terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&VoiceActivationLevelSliderHandler::reset, this, _1));
		}

		virtual ~VoiceActivationLevelSliderHandler() {}

	protected:
		VoiceChatHandlerPtr voiceChatHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->voiceActivationLevel = static_cast<int>(200.0f*pow(10.0f, static_cast<float>(position)/50.0f));
			this->terrorSave->save();

			this->voiceChatHandler->setSoundThreshold(this->terrorSave->voiceActivationLevel);
		}

		void reset(TerrorSave* terrorSave) {
			this->voiceChatHandler->setSoundThreshold(terrorSave->voiceActivationLevel);

			int position = static_cast<int>(50.0f*log10(static_cast<float>(terrorSave->voiceActivationLevel)/200.0f));
			this->setSliderPosition(position);
		}
	};

	typedef SharedPointer<VoiceActivationLevelSliderHandler> VoiceActivationLevelSliderHandlerPtr;
}

#endif
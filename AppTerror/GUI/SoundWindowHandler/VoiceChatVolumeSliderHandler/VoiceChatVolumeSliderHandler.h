#ifndef TERROR_VOICE_CHAT_VOLUME_SLIDER_HANDLER_H
#define TERROR_VOICE_CHAT_VOLUME_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class VoiceChatVolumeSliderHandler : public SliderHandler {
	public:
		VoiceChatVolumeSliderHandler(Widget* voiceChatVolumeSlider, TerrorSavePtr terrorSave)
			: SliderHandler(voiceChatVolumeSlider), terrorSave(terrorSave)
		{
			this->reset(this->terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&VoiceChatVolumeSliderHandler::reset, this, _1));
		}

		virtual ~VoiceChatVolumeSliderHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->voiceChatVolume = static_cast<float>(position);
			this->terrorSave->save();

			float realVoiceChatVolume = this->getRealVolume(this->terrorSave->voiceChatVolume);
			Core::getInstance()->setVolume(realVoiceChatVolume, "VoiceChat");
		}

		void reset(TerrorSave* terrorSave) {
			float realVoiceChatVolume = this->getRealVolume(this->terrorSave->voiceChatVolume);
			Core::getInstance()->setVolume(realVoiceChatVolume, "VoiceChat");

			this->setSliderPosition(this->terrorSave->voiceChatVolume);
		}

		float getRealVolume(float volume) {
			return (std::exp(2.0f*0.01f*volume) - 1.0f)/(std::exp(2.0f) - 1.0f);
		}
	};

	typedef SharedPointer<VoiceChatVolumeSliderHandler> VoiceChatVolumeSliderHandlerPtr;
}

#endif
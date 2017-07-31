#ifndef TERROR_VOICE_PITCH_SLIDER_HANDLER_H
#define TERROR_VOICE_PITCH_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"
#include "VoiceChatHandler/VoiceChatHandler.h"

namespace terror {
	class VoicePitchSliderHandler : public SliderHandler {
	public:
		VoicePitchSliderHandler(Widget* voicePitchSlider, VoiceChatHandlerPtr voiceChatHandler, TerrorSavePtr terrorSave)
			: SliderHandler(voicePitchSlider), voiceChatHandler(voiceChatHandler), terrorSave(terrorSave)
		{
			this->reset(terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&VoicePitchSliderHandler::reset, this, _1));
		}

		virtual ~VoicePitchSliderHandler() {}

	protected:
		VoiceChatHandlerPtr voiceChatHandler;
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->voicePitch = static_cast<float>(position)/40.0f + 0.75f;
			this->terrorSave->save();

			this->voiceChatHandler->setVoicePitch(this->terrorSave->voicePitch);
		}

		void reset(TerrorSave* terrorSave) {
			this->voiceChatHandler->setVoicePitch(terrorSave->voicePitch);			
			this->setSliderPosition(static_cast<int>(10.0f*terrorSave->voicePitch));
		}
	};

	typedef SharedPointer<VoicePitchSliderHandler> VoicePitchSliderHandlerPtr;
}

#endif
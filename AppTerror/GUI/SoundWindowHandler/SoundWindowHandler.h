#ifndef TERROR_SOUND_WINDOW_HANDLER_H
#define TERROR_SOUND_WINDOW_HANDLER_H

#include "MasterVolumeSliderHandler/MasterVolumeSliderHandler.h"
#include "GameVolumeSliderHandler/GameVolumeSliderHandler.h"
#include "GameMusicSliderHandler/GameMusicSliderHandler.h"
#include "VoiceChatVolumeSliderHandler/VoiceChatVolumeSliderHandler.h"
#include "CaptureModeComboboxHandler/CaptureModeComboboxHandler.h"
#include "VoicePitchSliderHandler/VoicePitchSliderHandler.h"
#include "VoiceActivationLevelSliderHandler/VoiceActivationLevelSliderHandler.h"

namespace terror {
	class SoundDefaultsButtonHandler : public ButtonHandler {
	public:
		SoundDefaultsButtonHandler(Widget* soundDefaultsButton, TerrorSavePtr terrorSave) 
			: ButtonHandler(soundDefaultsButton), terrorSave(terrorSave) {}

		virtual ~SoundDefaultsButtonHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleButtonClick() {
			this->terrorSave->loadSoundDataDefaults();
			this->terrorSave->save();
			this->terrorSave->notifyDataChange();
		}
	};
	typedef SharedPointer<SoundDefaultsButtonHandler> SoundDefaultsButtonHandlerPtr;

	class SoundWindowHandler : public InputHandler {
	public:
		SoundWindowHandler(GuiLayoutPtr soundLayout, VoiceChatHandlerPtr voiceChatHandler, TerrorSavePtr terrorSave)
			: soundLayout(soundLayout), terrorSave(terrorSave)
		{
			soundLayout->connectShowEvent(Bind(&VoiceChatHandler::startTesting, voiceChatHandler));
			soundLayout->connectHideEvent(Bind(&VoiceChatHandler::stopTesting, voiceChatHandler));

			Widget* masterVolumeSlider = soundLayout->getWidget("MasterVolumeScrollbar");
			this->masterVolumeSliderHandler = MasterVolumeSliderHandlerPtr(
				new MasterVolumeSliderHandler(masterVolumeSlider, terrorSave));

			Widget* gameVolumeSlider = soundLayout->getWidget("GameVolumeScrollbar");
			this->gameVolumeSliderHandler = GameVolumeSliderHandlerPtr(
				new GameVolumeSliderHandler(gameVolumeSlider, terrorSave));

			Widget* gameMusicSlider = soundLayout->getWidget("GameMusicScrollbar");
			this->gameMusicSliderHandler = GameMusicSliderHandlerPtr(
				new GameMusicSliderHandler(gameMusicSlider, terrorSave));

			Widget* voiceChatVolumeSlider = soundLayout->getWidget("VoiceChatVolumeScrollbar");
			this->voiceChatVolumeSliderHandler = VoiceChatVolumeSliderHandlerPtr(
				new VoiceChatVolumeSliderHandler(voiceChatVolumeSlider, terrorSave));

			Widget* captureModeCombobox = soundLayout->getWidget("CaptureModeCombobox");
			this->captureModeComboboxHandler = CaptureModeComboboxHandlerPtr(
				new CaptureModeComboboxHandler(captureModeCombobox, voiceChatHandler, terrorSave));

			Widget* voicePitchSlider = soundLayout->getWidget("VoicePitchScrollbar");
			this->voicePitchSliderHandler = VoicePitchSliderHandlerPtr(
				new VoicePitchSliderHandler(voicePitchSlider, voiceChatHandler, terrorSave));

			Widget* voiceActivationLevelSlider = soundLayout->getWidget("VoiceActivationLevelScrollbar");
			this->voiceActivationLevelSliderHandler = VoiceActivationLevelSliderHandlerPtr(
				new VoiceActivationLevelSliderHandler(voiceActivationLevelSlider, voiceChatHandler, terrorSave));

			this->soundDefaultsButtonHandler = SoundDefaultsButtonHandlerPtr(
				new SoundDefaultsButtonHandler(soundLayout->getWidget("DefaultsButton"), terrorSave));

			this->backButtonHandler = 
				BackButtonHandlerPtr(new BackButtonHandler(soundLayout->getWidget("BackButton"), soundLayout));
		}

		virtual ~SoundWindowHandler() {}

	protected:
		GuiLayoutPtr soundLayout;
		TerrorSavePtr terrorSave;
		MasterVolumeSliderHandlerPtr masterVolumeSliderHandler;
		GameVolumeSliderHandlerPtr gameVolumeSliderHandler;
		GameMusicSliderHandlerPtr gameMusicSliderHandler;
		VoiceChatVolumeSliderHandlerPtr voiceChatVolumeSliderHandler;
		CaptureModeComboboxHandlerPtr captureModeComboboxHandler;
		VoicePitchSliderHandlerPtr voicePitchSliderHandler;
		VoiceActivationLevelSliderHandlerPtr voiceActivationLevelSliderHandler;
		SoundDefaultsButtonHandlerPtr soundDefaultsButtonHandler;
		BackButtonHandlerPtr backButtonHandler;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (((button == BC_ESCAPE) || (button == this->terrorSave->controls->guiReturn)) && this->soundLayout->isVisible()) {
				this->soundLayout->hide();
			}
		}
	};

	typedef SharedPointer<SoundWindowHandler> SoundWindowHandlerPtr;
}

#endif
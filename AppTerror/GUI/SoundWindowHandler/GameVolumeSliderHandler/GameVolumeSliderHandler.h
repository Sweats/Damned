#ifndef TERROR_GAME_VOLUME_SLIDER_HANDLER_H
#define TERROR_GAME_VOLUME_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class GameVolumeSliderHandler : public SliderHandler {
	public:
		GameVolumeSliderHandler(Widget* gameVolumeSlider, TerrorSavePtr terrorSave)
			: SliderHandler(gameVolumeSlider), terrorSave(terrorSave)
		{
			this->reset(terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&GameVolumeSliderHandler::reset, this, _1));
		}

		virtual ~GameVolumeSliderHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->gameVolume = static_cast<float>(position);
			this->terrorSave->save();

			float realVolume = this->getRealVolume(this->terrorSave->gameVolume);
			Core::getInstance()->setVolume(realVolume, "Default");
		}

		void reset(TerrorSave* terrorSave) {
			float realVolume = this->getRealVolume(this->terrorSave->gameVolume);
			Core::getInstance()->setVolume(realVolume, "Default");

			this->setSliderPosition(this->terrorSave->gameVolume);					
		}

		float getRealVolume(float volume) {
			return (std::exp(2.0f*0.01f*volume) - 1.0f)/(std::exp(2.0f) - 1.0f);
		}
	};

	typedef SharedPointer<GameVolumeSliderHandler> GameVolumeSliderHandlerPtr;
}

#endif
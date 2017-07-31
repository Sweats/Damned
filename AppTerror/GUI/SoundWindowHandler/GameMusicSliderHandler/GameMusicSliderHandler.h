#ifndef TERROR_GAME_MUSIC_SLIDER_HANDLER_H
#define TERROR_GAME_MUSIC_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class GameMusicSliderHandler : public SliderHandler {
	public:
		GameMusicSliderHandler(Widget* gameMusicSlider, TerrorSavePtr terrorSave)
			: SliderHandler(gameMusicSlider), terrorSave(terrorSave)
		{
			this->reset(this->terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&GameMusicSliderHandler::reset, this, _1));
		}

		virtual ~GameMusicSliderHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->gameMusic = static_cast<float>(position);
			this->terrorSave->save();

			float realGameMusic = this->getRealVolume(this->terrorSave->gameMusic);
			Core::getInstance()->setVolume(realGameMusic, "AmbientMusic");
		}

		void reset(TerrorSave* terrorSave) {
			float realGameMusic = this->getRealVolume(this->terrorSave->gameMusic);
			Core::getInstance()->setVolume(realGameMusic, "AmbientMusic");

			this->setSliderPosition(this->terrorSave->gameMusic);
		}

		float getRealVolume(float volume) {
			return (std::exp(2.0f*0.01f*volume) - 1.0f)/(std::exp(2.0f) - 1.0f);
		}
	};

	typedef SharedPointer<GameMusicSliderHandler> GameMusicSliderHandlerPtr;
}

#endif
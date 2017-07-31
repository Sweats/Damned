#ifndef TERROR_MASTER_VOLUME_SLIDER_HANDLER_H
#define TERROR_MASTER_VOLUME_SLIDER_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class MasterVolumeSliderHandler : public SliderHandler {
	public:
		MasterVolumeSliderHandler(Widget* masterVolumeSlider, TerrorSavePtr terrorSave)
			: SliderHandler(masterVolumeSlider), terrorSave(terrorSave)
		{
			this->reset(terrorSave.get());
			this->terrorSave->connectDataChangedEvent(Bind(&MasterVolumeSliderHandler::reset, this, _1));
		}

		virtual ~MasterVolumeSliderHandler() {}

	protected:
		TerrorSavePtr terrorSave;

	private:
		virtual void handleSlider(int position) {
			this->terrorSave->masterVolume = static_cast<float>(position);
			this->terrorSave->save();

			float realVolume = this->getRealVolume(this->terrorSave->masterVolume);
			Core::getInstance()->setMasterVolume(realVolume);	
		}

		void reset(TerrorSave* terrorSave) {
			float realVolume = this->getRealVolume(this->terrorSave->masterVolume);
			Core::getInstance()->setMasterVolume(realVolume);

			this->setSliderPosition(this->terrorSave->masterVolume);		
		}

		float getRealVolume(float volume) {
			return (std::exp(2.0f*0.01f*volume) - 1.0f)/(std::exp(2.0f) - 1.0f);
		}
	};

	typedef SharedPointer<MasterVolumeSliderHandler> MasterVolumeSliderHandlerPtr;
}

#endif
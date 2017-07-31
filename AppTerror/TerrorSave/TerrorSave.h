#ifndef TERROR_TERROR_SAVE_H
#define TERROR_TERROR_SAVE_H

#include "Controls/Controls.h"

namespace terror {
	struct TerrorSave {		
		typedef Signal<void (TerrorSave*)> TerrorSaveSignal;

		friend class boost::serialization::access;

		TerrorSave(const std::string& saveFilename) : saveFilename(saveFilename) {}

		void save() {
			ofstream outputFile(this->saveFilename.c_str(), std::ios::out | std::ios::binary);
			if (outputFile.is_open()) {
				serializeObject(*this, outputFile);
			}
		}

		void load() {
			ifstream inputFile(this->saveFilename.c_str(), std::ios::in | std::ios::binary);
			if (!inputFile.is_open()) {
				this->initialize();
				this->save();
				return;
			}
			try {
				deserializeObject(inputFile, *this);
			}catch (std::exception& e) {
				this->initialize();
				this->save();
			}
		}

		void connectDataChangedEvent(const TerrorSaveSignal::slot_type& dataChangedEvent) {
			this->dataChangedSignal.connect(dataChangedEvent);
		}

		void notifyDataChange() {
			this->dataChangedSignal(this);
		}

		void loadSoundDataDefaults() {
			this->masterVolume = 100.0f;
			this->gameVolume = 50.0f;
			this->gameMusic = 100.0f;
			this->voiceChatVolume = 100.0f;
			this->useVoiceActivation = false;
			this->voiceActivationLevel = 2000;
			this->voicePitch = 1.0f;
		}

		void loadControlsDataDefaults() {
			if (!this->controls) {
				this->controls = ControlsPtr(new Controls);
			}
			this->controls->loadDefaults();
		}

		void setShadowQuality(const string& quality) {
			if (quality == "Very Low") {
				this->shadowQuality = 256;
			}
			if (quality == "Low") {
				this->shadowQuality = 512;
			}
			if (quality == "Medium") {
				this->shadowQuality = 1024;
			}
			if (quality == "High") {
				this->shadowQuality = 2048;
			}
			if (quality == "Very High") {
				this->shadowQuality = 4096;
			}
		}

		string getShadowQualityString() {
			if (this->shadowQuality == 256) {
				return "Very Low";
			}
			if (this->shadowQuality == 512) {
				return "Low";
			}
			if (this->shadowQuality == 1024) {
				return "Medium";
			}
			if (this->shadowQuality == 2048) {
				return "High";
			}
			if (this->shadowQuality == 4096) {
				return "Very High";
			}
			return "Unknown";
		}

		StringList getPossibleShadowQualityValues() {
			StringList values;
			values.push_back("Very Low");
			values.push_back("Low");
			values.push_back("Medium");
			values.push_back("High");
			values.push_back("Very High");
			return values;
		}

		void setFxaa(const string& fxaa) {
			if (fxaa == "Off") {
				this->fxaaQuality = 0;
			}
			if (fxaa == "Very Low") {
				this->fxaaQuality = 1;
			}
			if (fxaa == "Low") {
				this->fxaaQuality = 2;
			}
			if (fxaa == "Medium") {
				this->fxaaQuality = 3;
			}
			if (fxaa == "High") {
				this->fxaaQuality = 4;
			}
			if (fxaa == "Very High") {
				this->fxaaQuality = 5;
			}
		}

		string getFxaaString() {
			if (this->fxaaQuality == 0) {
				return "Off";
			}
			if (this->fxaaQuality == 1) {
				return "Very Low";
			}
			if (this->fxaaQuality == 2) {
				return "Low";
			}
			if (this->fxaaQuality == 3) {
				return "Medium";
			}
			if (this->fxaaQuality == 4) {
				return "High";
			}
			if (this->fxaaQuality == 5) {
				return "Very High";
			}
			return "Unknown";
		}

		StringList getPossibleFxaaValues() {
			StringList values;
			values.push_back("Off");
			values.push_back("Very Low");
			values.push_back("Low");
			values.push_back("Medium");
			values.push_back("High");
			values.push_back("Very High");
			return values;
		}

		std::string saveFilename;
		int version;

		ControlsPtr controls;
		
		float gamma;		
		int anisotropicFiltering;
		int shadowQuality;
		int fxaaQuality;
		bool dof;
		bool ssao;
		bool particleFx;
		bool volumetricFog;
		bool filmGrain;

		float masterVolume;
		float gameVolume;
		float gameMusic;
		float voiceChatVolume;
		bool useVoiceActivation;
		int voiceActivationLevel;
		float voicePitch;

		int fixedPort;
		bool useFixedPort;

	protected:
		TerrorSaveSignal dataChangedSignal;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->version;
			if (this->version != SAVE_VERSION) {
				throw std::runtime_error("Save file outdated!");
			}

			archive & this->controls;

			archive & this->gamma;			
			archive & this->anisotropicFiltering;
			archive & this->shadowQuality;
			archive & this->fxaaQuality;
			archive & this->dof;
			archive & this->ssao;
			archive & this->particleFx;
			archive & this->volumetricFog;			
			archive & this->filmGrain;			

			archive & this->masterVolume;
			archive & this->gameVolume;
			archive & this->gameMusic;
			archive & this->voiceChatVolume;
			archive & this->useVoiceActivation;
			archive & this->voiceActivationLevel;
			archive & this->voicePitch;

			archive & this->fixedPort;
			archive & this->useFixedPort;
		}

		void initialize() {
			this->version = SAVE_VERSION;

			this->loadControlsDataDefaults();

			this->gamma = 0.8f;			
			this->anisotropicFiltering = 4;
			this->shadowQuality = 1024;
			this->fxaaQuality = 3;
			this->dof = true;
			this->ssao = false;
			this->particleFx = false;
			this->volumetricFog = false;
			this->filmGrain = false;
			
			this->loadSoundDataDefaults();

			this->fixedPort = 55888;
			this->useFixedPort = false;
		}
	};

	typedef SharedPointer<TerrorSave> TerrorSavePtr;
}

#endif
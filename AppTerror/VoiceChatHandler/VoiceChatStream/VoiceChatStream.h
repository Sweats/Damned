#ifndef TERROR_VOICE_CHAT_STREAM_H
#define TERROR_VOICE_CHAT_STREAM_H

#include "Config.h"

namespace terror {
	class VoiceChatStream : public SoundStream {
	public:
		VoiceChatStream(SoundAttenuatorPtrList& soundAttenuators) 
			: SoundStream(16000, 1), soundAttenuators(soundAttenuators), soundDecoder(new SoundDecoder(16000)), mutex(new Mutex) {
			this->play();			
			this->setMinDistance(2.0f);
			this->setAttenuation(0.1f);
			this->setVolume(100.0f*SoundSystem::getMasterVolume()*Core::getInstance()->getVolume("VoiceChat"));
		}

		void disable3dAttenuation() {
			this->sf::SoundStream::setPosition(0.0f, 0.0f, 0.0f);
			this->setRelativeToListener(true);
			this->setVolume(100.0f*SoundSystem::getMasterVolume()*Core::getInstance()->getVolume("VoiceChat"));
		}

		void setPosition(const Vector3d& position) {
			this->sf::SoundStream::setPosition(toSfml(position));
			this->setRelativeToListener(false);

			Vector3d listenerPosition = toAlgebra(sf::Listener::getPosition());
			float attenuation = 1.0f;
			foreach (SoundAttenuatorPtr soundAttenuator, this->soundAttenuators) {
				attenuation *= soundAttenuator->computeAttenuation(position, listenerPosition);
			}
			this->setVolume(100.0f*SoundSystem::getMasterVolume()*Core::getInstance()->getVolume("VoiceChat"));
		}

		void addChunk(PackedSoundChunkPtr packedSoundChunk) {
			if (this->isRelativeToListener()) {
				this->setVolume(100.0f*SoundSystem::getMasterVolume()*Core::getInstance()->getVolume("VoiceChat"));
			}
			SoundChunkPtr soundChunk = this->soundDecoder->decode(packedSoundChunk);
			mutex->try_lock_for(boost::chrono::seconds(60));
			this->soundToPlay = soundChunk;
			mutex->unlock();
		}

		void stopStream() {
			this->stop();
		}

		virtual ~VoiceChatStream() {}

	protected:
		SoundAttenuatorPtrList& soundAttenuators;		
		SoundDecoderPtr soundDecoder;
		SoundChunkPtr soundToPlay;
		MutexPtr mutex;	

	private:
		virtual SoundChunkPtr getNextChunk() {
			mutex->try_lock_for(boost::chrono::seconds(60));
			SoundChunkPtr nextChunk = clone(this->soundToPlay);
			this->soundToPlay.reset();
			mutex->unlock();
			return nextChunk;
		}
	};

	typedef SharedPointer<VoiceChatStream> VoiceChatStreamPtr;
}

#endif
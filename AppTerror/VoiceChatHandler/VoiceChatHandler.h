#ifndef TERROR_VOICE_CHAT_HANDLER_H
#define TERROR_VOICE_CHAT_HANDLER_H

#include "Stage/Stage.h"
#include "CharacterHandler/CharacterHandler.h"
#include "VoiceChatStream/VoiceChatStream.h"
#include "Packets/GameSettings/GameSettings.h"

namespace terror {
	enum CaptureMode {
		VOICE_ACTIVATION,
		PUSH_TO_TALK
	};

	typedef PointerMap<int, VoiceChatStream> VoiceChatStreamPtrMap;
	typedef unordered_map<int, bool> BoolMap;

	class VoiceChatHandler : public EventHandler, public InputHandler, public NetworkObserver, public SoundRecorder {
	public:
		VoiceChatHandler(CharacterHandlerPtr characterHandler, ControlsPtr controls, TerrorClientPtr terrorClient)
			: EventHandler(60), SoundRecorder(1000), characterHandler(characterHandler), controls(controls), terrorClient(terrorClient), 
			soundEncoder(new SoundEncoder(16000, 5)), streamsMutex(new Mutex), chunksMutex(new Mutex), voicePitch(1.0f), ingame(false), testing(false)
		{
			this->chunksMutex->try_lock_for(boost::chrono::seconds(60));
			this->chunks = VoiceChatChunkVectorPtr(new VoiceChatChunkVector);
			this->chunks->clearChunks();
			this->chunksMutex->unlock();

			this->mutedPlayers[0] = false;
			this->mutedPlayers[1] = false;
			this->mutedPlayers[2] = false;
			this->mutedPlayers[3] = false;
			this->mutedPlayers[4] = false;
		}

		void setVoicePitch(float voicePitch) {
			this->voicePitch = voicePitch;
		}

		void startTesting() {
			this->testing = true;
		}

		void stopTesting() {
			this->testing = false;
			if (this->ownStream) {			
				this->ownStream->stopStream();
				this->ownStream.reset();
			}
		}

		void setCaptureMode(CaptureMode captureMode) {
			this->stop();
			this->captureMode = captureMode;
			if (this->captureMode == VOICE_ACTIVATION) {				
				this->enableVoiceActivation();
				this->start(16000);
			}else {
				this->disableVoiceActivation();
			}
		}

		void addSoundAttenuator(SoundAttenuatorPtr soundAttenuator) {
			this->soundAttenuators.add(soundAttenuator);
		}

		void notifyStageLoaded(StagePtr stage) {			
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->ingame = false;

			this->mutedPlayers[0] = false;
			this->mutedPlayers[1] = false;
			this->mutedPlayers[2] = false;
			this->mutedPlayers[3] = false;
			this->mutedPlayers[4] = false;
		}

		virtual ~VoiceChatHandler() {
			this->stop();

			this->streamsMutex->try_lock_for(boost::chrono::seconds(60));
			foreach (VoiceChatStreamPtrMap::value_type& entry, this->streams) {
				VoiceChatStreamPtr stream = entry.second;
				stream->stopStream();
			}
			this->streams.clear();
			this->streamsMutex->unlock();

			this->chunksMutex->try_lock_for(boost::chrono::seconds(60));
			this->chunks->clearChunks();
			this->chunksMutex->unlock();
		}

	protected:		
		CharacterHandlerPtr characterHandler;
		SoundAttenuatorPtrList soundAttenuators;		
		ControlsPtr controls;
		TerrorClientPtr terrorClient;
		SoundEncoderPtr soundEncoder;
		VoiceChatStreamPtrMap streams;
		VoiceChatStreamPtr ownStream;
		VoiceChatChunkVectorPtr chunks;
		MutexPtr streamsMutex;
		MutexPtr chunksMutex;
		CaptureMode captureMode;
		float voicePitch;
		bool ingame;
		bool testing;

		BoolMap mutedPlayers;

	private:
		virtual bool handleCapturedChunk(SoundChunkPtr capturedChunk) {
			if (this->testing) {
				PackedSoundChunkPtr packedSoundChunk = this->soundEncoder->encode(capturedChunk);
				if (!this->ownStream) {
					this->ownStream = VoiceChatStreamPtr(new VoiceChatStream(this->soundAttenuators));
					this->ownStream->disable3dAttenuation();
				}
				this->ownStream->setPitch(this->voicePitch);
				this->ownStream->addChunk(packedSoundChunk);
			}

			if (this->terrorClient->getId() == -1) {
				return true;
			}

			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getVoiceChatMode() == VOIP_DISABLED) {
				return true;
			}

			bool useVoicePitch = false;

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return true;
			}

			CharacterType thisChar = playerInfo->getCharacter();
			if (this->ingame) {
				if (thisChar == CHAR_NONE || thisChar == CHAR_RANDOM) {
					return true;
				}

				if (isMonsterCharacter(thisChar)) {
					useVoicePitch = true;
					MonsterPtr monster = this->characterHandler->getMonster();
					if (!monster) {
						return true;
					}
					if (monster->getType() == SPECTRUM) {
						SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
						if (!spectrum->isVisible()) {
							return true;
						}
					}
				}else {
					HumanPtr human = this->characterHandler->getHumanAlive(thisChar);
					if (!human || human->isDead()) {
						return true;
					}
				}
			}

			PackedSoundChunkPtr packedSoundChunk = this->soundEncoder->encode(capturedChunk);
			if (this->terrorClient->isHost()) {
				this->chunksMutex->try_lock_for(boost::chrono::seconds(60));
				VoiceChatChunkPtr chunk = (*this->chunks)[this->terrorClient->getId()];
				chunk->packedChunk = packedSoundChunk;
				if (useVoicePitch) {
					chunk->voicePitch = this->voicePitch;
				}else {
					chunk->voicePitch = 1.0f;
				}
				this->chunksMutex->unlock();
			}else {
				VoiceChatChunkPtr chunk(new VoiceChatChunk(this->terrorClient->getId()));
				chunk->packedChunk = packedSoundChunk;
				if (useVoicePitch) {
					chunk->voicePitch = this->voicePitch;
				}
				this->terrorClient->sendObjectToHost(chunk);
			}
			return true;
		}

		virtual bool handleEvent(float diffTime) {
			if (!this->terrorClient->isHost()) {
				return true;
			}
			this->chunksMutex->try_lock_for(boost::chrono::seconds(60));
			if (this->chunks->needSend()) {
				this->terrorClient->sendObjectToPlayers(this->chunks);
				this->chunks->clearChunks();			
			}
			this->chunksMutex->unlock();
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (button == BC_F6) {
				this->mutedPlayers[0] = !this->mutedPlayers[0];
			}
			if (button == BC_F7) {
				this->mutedPlayers[1] = !this->mutedPlayers[1];
			}
			if (button == BC_F8) {
				this->mutedPlayers[2] = !this->mutedPlayers[2];
			}
			if (button == BC_F9) {
				this->mutedPlayers[3] = !this->mutedPlayers[3];
			}
			if (button == BC_F10) {
				this->mutedPlayers[4] = !this->mutedPlayers[4];
			}
			if (this->captureMode != PUSH_TO_TALK) {
				return;
			}
			if (button == this->controls->pushToTalk) {
				this->start(16000);
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (this->captureMode != PUSH_TO_TALK) {
				return;
			}
			if (button == this->controls->pushToTalk) {
				this->stop();
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			// Host
			if (udpPacket->getLabel() == VOICE_CHAT_CHUNK) {
				VoiceChatChunkPtr chunk(new VoiceChatChunk);
				udpPacket->buildObject(chunk);
				this->chunksMutex->try_lock_for(boost::chrono::seconds(60));				
				(*this->chunks)[chunk->playerId] = chunk;
				this->chunksMutex->unlock();
				this->handleChunk(chunk);
			}

			// Client
			if (udpPacket->getLabel() == VOICE_CHAT_CHUNKS) {
				VoiceChatChunkVectorPtr chunks(new VoiceChatChunkVector);
				udpPacket->buildObject(chunks);
				foreach (VoiceChatChunkPtr chunk, *chunks) {
					this->handleChunk(chunk);
				}
			}
		}

		void handleChunk(VoiceChatChunkPtr chunk) {
			if (chunk->isEmpty() || chunk->playerId == this->terrorClient->getId() || this->mutedPlayers[chunk->playerId]) {
				return;
			}			
			
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getVoiceChatMode() == VOIP_DISABLED) {
				return;
			}

			if (!this->ingame || gameSettings->getVoiceChatMode() == VOIP_SIMPLE) {
				this->streamsMutex->try_lock_for(boost::chrono::seconds(60));
				if (!this->streams.contains(chunk->playerId)) {
					this->streams[chunk->playerId] = VoiceChatStreamPtr(new VoiceChatStream(this->soundAttenuators));
				}
				this->streams[chunk->playerId]->disable3dAttenuation();
				this->streams[chunk->playerId]->setPitch(chunk->voicePitch);
				this->streams[chunk->playerId]->addChunk(chunk->packedChunk);
				this->streamsMutex->unlock();
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			CharacterType thisChar =  playerInfo->getCharacter();
			if (thisChar == CHAR_NONE || thisChar == CHAR_RANDOM) {
				return;
			}

			if (isMonsterCharacter(thisChar)) {
				MonsterPtr monster = this->characterHandler->getMonster();
				if (!monster) {
					return;
				}
				if (monster->getType() == SPECTRUM) {
					SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
					if (!spectrum->isVisible()) {
						return;
					}
				}else {
					return;
				}
			}

			CharacterType remoteChar = playersState->getPlayerInfo(chunk->playerId)->getCharacter();
			CharacterPtr character = this->characterHandler->getCharacter(remoteChar);
			if (!character) {
				return;
			}
			Vector3d position = character->getActor()->getPosition();

			this->streamsMutex->try_lock_for(boost::chrono::seconds(60));
			if (!this->streams.contains(chunk->playerId)) {
				this->streams[chunk->playerId] = VoiceChatStreamPtr(new VoiceChatStream(this->soundAttenuators));
			}
			this->streams[chunk->playerId]->setPosition(position);
			this->streams[chunk->playerId]->setPitch(chunk->voicePitch);
			this->streams[chunk->playerId]->addChunk(chunk->packedChunk);
			this->streamsMutex->unlock();
		}
	};

	typedef SharedPointer<VoiceChatHandler> VoiceChatHandlerPtr;
}

#endif
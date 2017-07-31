#ifndef TERROR_VOICE_CHAT_CHUNK_H
#define TERROR_VOICE_CHAT_CHUNK_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct VoiceChatChunk : public Sendable {
		friend class boost::serialization::access;

		VoiceChatChunk() : packedChunk(new PackedSoundChunk), voicePitch(1.0f), playerId(-1) {}
		VoiceChatChunk(int playerId) : packedChunk(new PackedSoundChunk), voicePitch(1.0f), playerId(playerId) {}

		virtual int getLabel() const {
			return VOICE_CHAT_CHUNK;
		}

		void clear() {
			this->packedChunk->clear();
		}

		bool isEmpty() const {
			return this->packedChunk->isEmpty();
		}

		virtual ~VoiceChatChunk() {}

		PackedSoundChunkPtr packedChunk;
		float voicePitch;
		int playerId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->packedChunk;
			archive & this->voicePitch;
			archive & this->playerId;
		}
	};

	typedef SharedPointer<VoiceChatChunk> VoiceChatChunkPtr;
}

#endif
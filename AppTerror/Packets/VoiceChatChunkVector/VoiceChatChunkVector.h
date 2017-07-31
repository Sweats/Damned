#ifndef TERROR_VOICE_CHAT_CHUNK_VECTOR_H
#define TERROR_VOICE_CHAT_CHUNK_VECTOR_H

#include "Packets/VoiceChatChunk/VoiceChatChunk.h"

namespace terror {
	struct VoiceChatChunkVector : public Sendable, public std::vector<VoiceChatChunkPtr> {
		friend class boost::serialization::access;

		VoiceChatChunkVector() {
			this->resize(5);
			for (int i = 0; i < 5; ++i) {
				(*this)[i] = VoiceChatChunkPtr(new VoiceChatChunk);
				(*this)[i]->playerId = i;
			}
		}

		virtual int getLabel() const {
			return VOICE_CHAT_CHUNKS;
		}

		void clearChunks() {
			foreach (VoiceChatChunkPtr chunk, *this) {
				chunk->clear();
			}
		}

		bool needSend() {
			foreach (VoiceChatChunkPtr chunk, *this) {
				if (!chunk->isEmpty()) {
					return true;
				}
			}
			return false;
		}

		virtual ~VoiceChatChunkVector() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & BaseObject<std::vector<VoiceChatChunkPtr> >(*this);
		}
	};

	typedef SharedPointer<VoiceChatChunkVector> VoiceChatChunkVectorPtr;
}

#endif
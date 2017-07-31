#ifndef TERROR_KILL_CHARACTER_REQUEST_H
#define TERROR_KILL_CHARACTER_REQUEST_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class KillCharacterRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		KillCharacterRequest() {}
		KillCharacterRequest(CharacterType character) : character(character) {}
	
		virtual int getLabel() const {
			return KILL_CHARACTER_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		virtual ~KillCharacterRequest() {}

	protected:
		CharacterType character;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
		}
	};

	typedef SharedPointer<KillCharacterRequest> KillCharacterRequestPtr;
}

#endif
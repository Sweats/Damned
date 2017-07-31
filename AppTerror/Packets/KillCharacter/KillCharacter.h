#ifndef TERROR_KILL_CHARACTER_H
#define TERROR_KILL_CHARACTER_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class KillCharacter : public Sendable {
	public:
		friend class boost::serialization::access;

		KillCharacter() : numberOfLives(1) {}
		KillCharacter(CharacterType character, int numberOfLives) : character(character), numberOfLives(numberOfLives) {}
	
		virtual int getLabel() const {
			return KILL_CHARACTER;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getNumberOfLives() {
			return this->numberOfLives;
		}

		virtual ~KillCharacter() {}

	protected:
		CharacterType character;
		int numberOfLives;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->numberOfLives;
		}
	};
	
	typedef SharedPointer<KillCharacter> KillCharacterPtr;
}

#endif
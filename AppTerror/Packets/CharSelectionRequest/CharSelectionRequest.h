#ifndef TERROR_CHAR_SELECTION_REQUEST_H
#define TERROR_CHAR_SELECTION_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {
	enum CharacterType {
		CHAR_0,
		CHAR_1,
		CHAR_2,
		CHAR_3,
		CHAR_MONSTER_0,
		CHAR_MONSTER_1,
		CHAR_MONSTER_2,
		CHAR_MONSTER_3,
		CHAR_RANDOM,
		CHAR_NONE
	};

	bool isMonsterCharacter(CharacterType character) {
		return (character == CHAR_MONSTER_0 || character == CHAR_MONSTER_1 || character == CHAR_MONSTER_2 || character == CHAR_MONSTER_3);
	}

	bool isHumanCharacter(CharacterType character) {
		return (character != CHAR_NONE && character != CHAR_RANDOM && !isMonsterCharacter(character)); 
	}

	class CharSelectionRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		CharSelectionRequest() {}

		CharSelectionRequest(CharacterType character, int id) : character(character), id(id) {}

		virtual int getLabel() const {
			return CHAR_SELECTION_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getId() {
			return this->id;
		}

		virtual ~CharSelectionRequest() {}

	protected:
		CharacterType character;
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->id;
		}
	};

	typedef SharedPointer<CharSelectionRequest> CharSelectionRequestPtr;
}

#endif
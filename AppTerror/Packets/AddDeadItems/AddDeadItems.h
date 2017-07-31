#ifndef TERROR_ADD_DEAD_ITEMS_H
#define TERROR_ADD_DEAD_ITEMS_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class AddDeadItems : public Sendable {
	public:
		friend class boost::serialization::access;

		AddDeadItems() {}
		AddDeadItems(CharacterType character, int deadId) : character(character), deadId(deadId) {}
	
		virtual int getLabel() const {
			return ADD_DEAD_ITEMS;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getDeadId() {
			return this->deadId;
		}

		virtual ~AddDeadItems() {}

	protected:
		CharacterType character;
		int deadId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->deadId;
		}
	};

	typedef SharedPointer<AddDeadItems> AddDeadItemsPtr;
}

#endif
#ifndef TERROR_ADD_ITEM_H
#define TERROR_ADD_ITEM_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class AddItem : public Sendable {
	public:
		friend class boost::serialization::access;

		AddItem() {}
		AddItem(CharacterType character, int objectId) : character(character), objectId(objectId) {}
	
		virtual int getLabel() const {
			return ADD_ITEM;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getObjectId() {
			return this->objectId;
		}

		virtual ~AddItem() {}

	protected:
		CharacterType character;
		int objectId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->objectId;
		}
	};

	typedef SharedPointer<AddItem> AddItemPtr;
}

#endif
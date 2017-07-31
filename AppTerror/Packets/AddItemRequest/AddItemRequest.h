#ifndef TERROR_ADD_ITEM_REQUEST_H
#define TERROR_ADD_ITEM_REQUEST_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class AddItemRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		AddItemRequest() {}
		AddItemRequest(CharacterType character, int objectId) : character(character), objectId(objectId) {}
	
		virtual int getLabel() const {
			return ADD_ITEM_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getObjectId() {
			return this->objectId;
		}

		virtual ~AddItemRequest() {}

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

	typedef SharedPointer<AddItemRequest> AddItemRequestPtr;
}

#endif
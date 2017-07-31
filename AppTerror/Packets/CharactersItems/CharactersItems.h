#ifndef TERROR_CHARACTERS_ITEMS_H
#define TERROR_CHARACTERS_ITEMS_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {
	class CharacterItems {
	public:
		friend class boost::serialization::access;

		CharacterItems() {}
		CharacterItems(CharacterType character) : character(character) {}

		CharacterType getCharacter() {
			return this->character;
		}

		void addItemMask(int itemMask) {
			this->itemMasks.push_back(itemMask);
		}

		IntList& getItemMasks() {
			return this->itemMasks;
		}

		virtual ~CharacterItems() {}

	protected:
		CharacterType character;
		IntList itemMasks;
		
	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->itemMasks;
		}
	};
	typedef SharedPointer<CharacterItems> CharacterItemsPtr;

	typedef PointerList<CharacterItems> CharacterItemsPtrList;

	class CharactersItems : public Sendable {
	public:
		friend class boost::serialization::access;

		CharactersItems() {}
	
		virtual int getLabel() const {
			return CHARACTERS_ITEMS;
		}

		void addCharacterItems(CharacterItemsPtr characterItems) {
			this->charactersItems.add(characterItems);
		}

		CharacterItemsPtrList getCharactersItems() {
			return this->charactersItems;
		}

		virtual ~CharactersItems() {}

	protected:
		CharacterItemsPtrList charactersItems;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->charactersItems;
		}
	};

	typedef SharedPointer<CharactersItems> CharactersItemsPtr;
}

#endif
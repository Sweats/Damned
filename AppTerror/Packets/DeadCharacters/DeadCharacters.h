#ifndef TERROR_DEAD_CHARACTERS_H
#define TERROR_DEAD_CHARACTERS_H

#include "Packets/DeadHumanData/DeadHumanData.h"

namespace terror {
	typedef PointerList<DeadHumanData> DeadHumanDataPtrList;

	class DeadCharacters : public Sendable {
	public:
		friend class boost::serialization::access;

		DeadCharacters() : numberOfLives(1) {}
		DeadCharacters(int numberOfLives) : numberOfLives(numberOfLives) {}

		virtual int getLabel() const {
			return DEAD_CHARACTERS;
		}

		void addDeadCharacterData(DeadHumanDataPtr deadHumanData) {
			this->deadCharactersData.add(deadHumanData);
		}

		DeadHumanDataPtrList getDeadCharactersData() {
			return this->deadCharactersData;
		}

		int getNumberOfLives() {
			return this->numberOfLives;
		}

		virtual ~DeadCharacters() {}

	protected:
		DeadHumanDataPtrList deadCharactersData;
		int numberOfLives;
		
	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->deadCharactersData;
			archive & this->numberOfLives;
		}
	};

	typedef SharedPointer<DeadCharacters> DeadCharactersPtr;
}

#endif
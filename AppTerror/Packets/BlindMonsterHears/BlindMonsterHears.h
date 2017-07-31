#ifndef TERROR_BLIND_MONSTER_HEARS_H
#define TERROR_BLIND_MONSTER_HEARS_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {
	typedef std::list<CharacterType> CharacterTypeList;

	struct BlindMonsterHears : public Sendable {
		friend class boost::serialization::access;
		
		BlindMonsterHears() {}

		virtual int getLabel() const {
			return BLIND_MONSTER_HEARS;
		}

		virtual ~BlindMonsterHears() {}

		CharacterTypeList characters;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->characters;
		}
	};	

	typedef SharedPointer<BlindMonsterHears> BlindMonsterHearsPtr;
}

#endif
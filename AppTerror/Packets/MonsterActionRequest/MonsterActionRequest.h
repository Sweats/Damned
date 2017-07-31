#ifndef TERROR_MONSTER_ACTION_REQUEST_H
#define TERROR_MONSTER_ACTION_REQUEST_H

#include "Packets/PacketsEnum.h"
#include "Packets/Vector3dInfo/Vector3dInfo.h"
#include "Packets/QuaternionInfo/QuaternionInfo.h"
#include "Packets/CharacterStateInfo/CharacterStateInfo.h"

namespace terror { 
	struct MonsterActionRequest : public Sendable {
		friend class boost::serialization::access;

		MonsterActionRequest() : stateInfo(new CharacterStateInfo) {}

		virtual int getLabel() const {
			return MONSTER_ACTION_REQUEST;
		}

		virtual ~MonsterActionRequest() {}

		CharacterStateInfoPtr stateInfo;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->stateInfo;
		}
	};

	typedef SharedPointer<MonsterActionRequest> MonsterActionRequestPtr;
}

#endif
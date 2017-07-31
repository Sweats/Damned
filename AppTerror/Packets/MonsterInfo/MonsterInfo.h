#ifndef TERROR_MONSTER_INFO_H
#define TERROR_MONSTER_INFO_H

#include "Packets/MonsterActionRequest/MonsterActionRequest.h"

namespace terror {	
	struct MonsterInfo {
		friend class boost::serialization::access;

		MonsterInfo() : stateInfo(new CharacterStateInfo) {}

		void update(MonsterActionRequestPtr monsterActionRequest) {
			this->stateInfo = monsterActionRequest->stateInfo;
		}

		virtual ~MonsterInfo() {}

		CharacterStateInfoPtr stateInfo;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {		
			archive & this->stateInfo;
		}
	};

	typedef SharedPointer<MonsterInfo> MonsterInfoPtr;
}

#endif
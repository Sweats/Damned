#ifndef TERROR_BERSERKER_MONSTER_TELEPORT_REQUEST_H
#define TERROR_BERSERKER_MONSTER_TELEPORT_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class BerserkerMonsterTeleportRequest : public Sendable {
	public:
		friend class boost::serialization::access;
	
		BerserkerMonsterTeleportRequest(bool autoTeleport = false) : autoTeleport(autoTeleport) {}

		virtual int getLabel() const {
			return BERSERKER_MONSTER_TELEPORT_REQUEST;
		}

	protected:
		bool autoTeleport;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<BerserkerMonsterTeleportRequest> BerserkerMonsterTeleportRequestPtr;
}

#endif
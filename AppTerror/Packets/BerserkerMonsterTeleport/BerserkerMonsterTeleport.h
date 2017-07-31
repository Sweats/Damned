#ifndef TERROR_BERSERKER_MONSTER_TELEPORT_H
#define TERROR_BERSERKER_MONSTER_TELEPORT_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class BerserkerMonsterTeleport : public Sendable {
	public:
		friend class boost::serialization::access;
	
		BerserkerMonsterTeleport() {}
		BerserkerMonsterTeleport(float energy, int portalId, bool autoTeleport = false) : energy(energy), portalId(portalId), autoTeleport(autoTeleport) {}

		virtual int getLabel() const {
			return BERSERKER_MONSTER_TELEPORT;
		}

		float getEnergy() {
			return this->energy;
		}

		int getPortalId() {
			return this->portalId;
		}

		bool isAutoTeleport() {
			return this->autoTeleport;
		}

	protected:
		float energy;
		int portalId;
		bool autoTeleport;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->energy;
			archive & this->portalId;
			archive & this->autoTeleport;
		}
	};

	typedef SharedPointer<BerserkerMonsterTeleport> BerserkerMonsterTeleportPtr;
}

#endif
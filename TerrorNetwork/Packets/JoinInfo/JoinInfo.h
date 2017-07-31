#ifndef TERROR_NETWORK_JOIN_INFO_H
#define TERROR_NETWORK_JOIN_INFO_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class JoinInfo : public Sendable {
	public:
		friend class boost::serialization::access;

		JoinInfo() {}

		JoinInfo(int playerId, int hostUuid, int oldHostId = -1) 
			: playerId(playerId), hostUuid(hostUuid), oldHostId(oldHostId) {}

		virtual int getLabel() const {
			return JOIN_INFO;
		}

		int getPlayerId() const {
			return this->playerId;
		}

		int getHostUuid() {
			return this->hostUuid;
		}

		int getOldHostId() {
			return this->oldHostId;
		}

		virtual ~JoinInfo() {}

	protected:
		int playerId;
		int hostUuid;
		int oldHostId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->playerId;
			archive & this->hostUuid;
			archive & this->oldHostId;
		}
	};

	typedef SharedPointer<JoinInfo> JoinInfoPtr;
}

#endif
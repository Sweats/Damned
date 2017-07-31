#ifndef TERROR_NETWORK_LEAVE_GAME_REQUEST_H
#define TERROR_NETWORK_LEAVE_GAME_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class LeaveGameRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		LeaveGameRequest() {}

		virtual int getLabel() const {
			return LEAVE_GAME_REQUEST;
		}

		virtual ~LeaveGameRequest() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<LeaveGameRequest> LeaveGameRequestPtr;
}

#endif
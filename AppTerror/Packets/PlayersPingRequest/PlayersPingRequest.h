#ifndef TERROR_PLAYERS_PING_REQUEST_H
#define TERROR_PLAYERS_PING_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class PlayersPingRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		PlayersPingRequest() {}

		virtual int getLabel() const {
			return PLAYERS_PING_REQUEST;
		}

		virtual ~PlayersPingRequest() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<PlayersPingRequest> PlayersPingRequestPtr;
}

#endif
#ifndef TERROR_PLAYERS_STATE_ACK_H
#define TERROR_PLAYERS_STATE_ACK_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class PlayersStateAck : public Sendable {
	public:
		friend class boost::serialization::access;

		PlayersStateAck() {}

		virtual int getLabel() const {
			return PLAYERS_STATE_ACK;
		}

		virtual ~PlayersStateAck() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<PlayersStateAck> PlayersStateAckPtr;
}

#endif
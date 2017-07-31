#ifndef TERROR_KICK_PLAYER_H
#define TERROR_KICK_PLAYER_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct KickPlayer : public Sendable {
		friend class boost::serialization::access;

		KickPlayer() {}
		KickPlayer(int playerId) : playerId(playerId) {}

		virtual int getLabel() const {
			return KICK_PLAYER;
		}

		virtual ~KickPlayer() {}

		int playerId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->playerId;
		}
	};

	typedef SharedPointer<KickPlayer> KickPlayerPtr;
}

#endif
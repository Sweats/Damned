#ifndef TERROR_PLAYER_JOINED_GAME_H
#define TERROR_PLAYER_JOINED_GAME_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class PlayerJoinedGame : public Sendable {
	public:
		friend class boost::serialization::access;
		
		PlayerJoinedGame() {}
		PlayerJoinedGame(int playerId) : playerId(playerId) {}

		virtual int getLabel() const {
			return PLAYER_JOINED_GAME;
		}

		int getPlayerId() {
			return this->playerId;
		}

		virtual ~PlayerJoinedGame() {}

	protected:
		int playerId;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->playerId;
		}
	};

	typedef SharedPointer<PlayerJoinedGame> PlayerJoinedGamePtr;
}

#endif
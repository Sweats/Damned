#ifndef TERROR_NETWORK_GAME_ENDED_H
#define TERROR_NETWORK_GAME_ENDED_H

#include "Packets/PacketsEnum.h"

namespace terror_network{
	enum GameEndedStatus {
		TEAM_1_WON,
		TEAM_2_WON
	};

	class GameEnded : public Sendable {
	public:
		friend class boost::serialization::access;

		GameEnded() {}

		GameEnded(GameEndedStatus gameEndedStatus) : gameEndedStatus(gameEndedStatus) {}

		virtual int getLabel() const {
			return GAME_ENDED;
		}

		GameEndedStatus getStatus() {
			return this->gameEndedStatus;
		}

		virtual ~GameEnded() {}

	protected:		
		GameEndedStatus gameEndedStatus;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {			
			archive & this->gameEndedStatus;
		}
	};

	typedef SharedPointer<GameEnded> GameEndedPtr;
}

#endif
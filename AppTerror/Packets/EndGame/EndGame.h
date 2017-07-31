#ifndef TERROR_END_GAME_H
#define TERROR_END_GAME_H

#include "Packets/PacketsEnum.h"

namespace terror {
	enum EndGameStatus {
		GAME_CANCELED,
		MONSTER_WINS,
		SURVIVORS_WINS
	};

	struct EndGame : public Sendable {
		friend class boost::serialization::access;
		
		EndGame() {}
		EndGame(EndGameStatus status) : status(status) {}

		virtual int getLabel() const {
			return END_GAME;
		}

		EndGameStatus status;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->status;
		}
	};	

	typedef SharedPointer<EndGame> EndGamePtr;
}

#endif
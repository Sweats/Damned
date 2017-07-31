#ifndef TERROR_START_GAME_H
#define TERROR_START_GAME_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct StartGame : public Sendable {
		friend class boost::serialization::access;
		
		StartGame() {}

		virtual int getLabel() const {
			return START_GAME;
		}

		virtual ~StartGame() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};	

	typedef SharedPointer<StartGame> StartGamePtr;
}

#endif
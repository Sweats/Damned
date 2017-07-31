#ifndef TERROR_NETWORK_GAME_SESSION_LIST_REQUEST_H
#define TERROR_NETWORK_GAME_SESSION_LIST_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class GameSessionListRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		GameSessionListRequest() {}

		virtual int getLabel() const {
			return GAME_LIST_REQUEST;
		}

		virtual ~GameSessionListRequest() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<GameSessionListRequest> GameSessionListRequestPtr;
}

#endif
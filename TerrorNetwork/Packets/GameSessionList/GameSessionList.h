#ifndef TERROR_NETWORK_GAME_SESSION_LIST_H
#define TERROR_NETWORK_GAME_SESSION_LIST_H

#include "Packets/PacketsEnum.h"
#include "GameSession/GameSession.h"

namespace terror_network {
	typedef IntPointerMap<GameSession> GameSessionPtrMap;

	class GameSessionList : public Sendable, public PointerList<GameSessionInfo> {
	public:
		friend class boost::serialization::access;

		GameSessionList() {}

		GameSessionList(GameSessionPtrMap& allGameSessions) {
			int maxGames = 512;
			foreach (GameSessionPtrMap::value_type& entry, allGameSessions) {
				GameSessionPtr gameSession = entry.second;
				if (gameSession) {
					--maxGames;
					if (maxGames < 0) {
						break;
					}
					this->add(gameSession->getInfo());					
				}
			}
		}

		virtual int getLabel() const {
			return GAME_LIST;
		}

		virtual ~GameSessionList() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & BaseObject<PointerList<GameSessionInfo> >(*this);
		}
	};

	typedef SharedPointer<GameSessionList> GameSessionListPtr;
}

#endif
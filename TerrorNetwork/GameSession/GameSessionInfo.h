#ifndef TERROR_NETWORK_GAME_SESSION_INFO_H
#define TERROR_NETWORK_GAME_SESSION_INFO_H

#include "Config.h"

namespace terror_network {
	class GameSessionInfo {
	public:
		friend class boost::serialization::access;

		GameSessionInfo() {}

		GameSessionInfo(const string& name, const string& stageName, int nPlayers, int maxPlayers, int id, UserInfoPtr hostInfo, bool privateSession) 
			: name(name), stageName(stageName), nPlayers(nPlayers), maxPlayers(maxPlayers), privateSession(privateSession), id(id), hostInfo(hostInfo) {}

		string getName() const {
			return this->name;
		}

		string getStageName() const {
			return this->stageName;
		}
		
		bool isPrivate() const {
			return this->privateSession;
		}

		int getNumberOfPlayers() const {
			return this->nPlayers;
		}

		int getMaxPlayers() const {
			return this->maxPlayers;
		}

		int getId() const {
			return this->id;
		}

		UserInfoPtr getHostInfo() const {
			return this->hostInfo;
		}

		virtual ~GameSessionInfo() {}

	protected:
		string name;
		string stageName;
		bool privateSession;
		int nPlayers;
		int maxPlayers;
		int id;
		UserInfoPtr hostInfo;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
			archive & this->stageName;
			archive & this->privateSession;
			archive & this->nPlayers;
			archive & this->maxPlayers;
			archive & this->id;	
			archive & this->hostInfo;
		}
	};
	
	typedef SharedPointer<GameSessionInfo> GameSessionInfoPtr;
}

#endif
#ifndef TERROR_NETWORK_GAME_SESSION_H
#define TERROR_NETWORK_GAME_SESSION_H

#include "GameSessionInfo.h"
#include "Packets/RemovePlayer/RemovePlayer.h"
#include "Packets/HostInfo/HostInfo.h"
#include "Packets/JoinInfo/JoinInfo.h"

namespace terror_network {
	typedef PointerMap<int, ConnectedUser> ConnectedUserPtrMap;

	class GameSession {
	public:
		GameSession(ConnectedUserPtr host, const string& name, const string& password, int nPrivateSlots, int maxPlayers);
		
		void setId(int id);
		void setStageName(const string& stageName);
		ConnectedUserPtr getHost();		
		ConnectedUserPtr getPlayer(int uuid);
		int addPlayer(ConnectedUserPtr player);		
		void removePlayer(int uuid);		
		bool hasPlayer(int uuid);
		
		GameSessionInfoPtr getInfo();	
		int getNumberOfPlayers() const;
		
		bool validatePassword(const string& passwordAttempt) const;
		bool isFull() const;
		int getId() const;

		virtual ~GameSession();

	protected:
		string name;
		string password;
		string stageName;
		ConnectedUserPtrMap players;	
		int nPrivateSlots;
		int maxPlayers;
		int hostId;
		int id;

	private:
		void transferHost();
		void notifyPlayerRemoval(int playerId);
	};

	typedef SharedPointer<GameSession> GameSessionPtr;
}

#endif

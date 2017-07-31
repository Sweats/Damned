#include "GameSession.h"

namespace terror_network {
	GameSession::GameSession(ConnectedUserPtr host, const string& name, const string& password, int nPrivateSlots, int maxPlayers)
		: name(name), password(password), stageName(""), nPrivateSlots(nPrivateSlots), maxPlayers(maxPlayers), 
		hostId(0)
	{
		this->players[0] = host;
		this->hostId = 0;
	}

	void GameSession::setId(int id) {
		this->id = id;
	}

	void GameSession::setStageName(const string& stageName) {
		this->stageName = stageName;
	}

	int GameSession::addPlayer(ConnectedUserPtr player) {
		for (int id = 0; id < this->maxPlayers; ++id) {
			if (!this->players.contains(id)) {
				this->players[id] = player;
				return id;
			}
		}
		return -1;
	}

	void GameSession::removePlayer(int uuid) {
		int playerId = -1;
		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			ConnectedUserPtr player = entry.second;
			if (player->getUuid() == uuid) {
				playerId = entry.first;
				this->players.erase(playerId);
				break;
			}
		}
		if (playerId == -1) {
			throw std::runtime_error("Couldn't find player to remove from game session!");
		}
		if (playerId == this->hostId) {
			this->transferHost();
		}else {
			this->notifyPlayerRemoval(playerId);
		}		
	}	

	ConnectedUserPtr GameSession::getHost() {
		return this->players[this->hostId];
	}

	ConnectedUserPtr GameSession::getPlayer(int uuid) {
		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			ConnectedUserPtr player = entry.second;
			if (player->getUuid() == uuid) {
				return player;
			}
		}
		return ConnectedUserPtr();
	}

	GameSessionInfoPtr GameSession::getInfo() {
		ConnectedUserPtr host = this->getHost();
		UserInfoPtr hostInfo = host->getUserInfo();
		bool isPrivate = (this->password != "");
		return GameSessionInfoPtr(new GameSessionInfo(this->name, this->stageName, this->getNumberOfPlayers(), this->maxPlayers, this->id, hostInfo, isPrivate));
	}

	int GameSession::getId() const {
		return this->id;
	}

	int GameSession::getNumberOfPlayers() const {
		return this->players.size();
	}

	bool GameSession::hasPlayer(int uuid) {
		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			ConnectedUserPtr player = entry.second;
			if (player->getUuid() == uuid) {
				return true;
			}
		}
		return false;
	}

	bool GameSession::validatePassword(const string& passwordAttempt) const {
		return (passwordAttempt == this->password);
	}

	bool GameSession::isFull() const {
		return (this->getNumberOfPlayers() >= this->maxPlayers);
	}

	GameSession::~GameSession() {}

	void GameSession::transferHost() {
		int oldHostId = this->hostId;
		this->hostId = this->players.begin()->first;

		ConnectedUserPtr host = this->getHost();
		HostInfoPtr hostInfo(new HostInfo(oldHostId));
		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			if (entry.first != this->hostId) {
				ConnectedUserPtr player = entry.second;
				UserInfoPtr playerInfo = player->getUserInfo();
				hostInfo->addUserInfo(playerInfo);	
			}
		}
		host->sendObject(hostInfo);

		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			if (entry.first != this->hostId) {
				ConnectedUserPtr player = entry.second;
				JoinInfoPtr joinInfo(new JoinInfo(entry.first, host->getUuid(), oldHostId));
				player->sendObject(joinInfo);
			}
		}
	}

	void GameSession::notifyPlayerRemoval(int playerId) {
		RemovePlayerPtr removePlayer(new RemovePlayer(playerId));
		foreach (ConnectedUserPtrMap::value_type& entry, this->players) {
			ConnectedUserPtr player = entry.second;
			player->sendObject(removePlayer);
		}
	}
}
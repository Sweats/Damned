#include "TerrorClient.h"

namespace terror_network {
	TerrorClient::TerrorClient(UdpPunchThroughClientPtr udpPunchThroughClient, UdpAddressPtr server) 
		: EventHandler(60), udpPunchThroughClient(udpPunchThroughClient), server(server),
		packetsMutex(new Mutex), playerId(new PlayerId(-1)), hosting(false) 
	{
		this->udpPunchThroughClient->connectUdpPunchThroughSuccessEvent(Bind(&TerrorClient::notifyUdpPunchThroughSuccess, this, _1, _2));
		this->udpPunchThroughClient->connectUdpPunchThroughFailEvent(Bind(&TerrorClient::notifyUdpPunchThroughFail, this, _1, _2));
	}	

	void TerrorClient::addNetworkObserver(NetworkObserverPtr messageObserver) {
		this->messageObservers.add(messageObserver);
	}

	void TerrorClient::removeNetworkObserver(NetworkObserverPtr messageObserver) {
		this->messageObservers.erase(messageObserver);
	}	

	int TerrorClient::getId() {
		return this->playerId->getId();
	}

	bool TerrorClient::isHost() const {
		return this->hosting;
	}

	void TerrorClient::reset() {			
		this->getUdpConnection()->reset();
		this->playerId->setId(-1);
		this->host.reset();
		this->players.clear();
		this->hosting = false;
	}

	TerrorClient::~TerrorClient() {}

	bool TerrorClient::handleEvent(float diffTime) {
		this->packetsMutex->try_lock_for(boost::chrono::seconds(60));
		foreach (NetworkObserverPtr messageObserver, this->messageObservers) {
			if (this->playerId->getId() != -1 || !messageObserver->isIngameOnly()) {
				foreach (TcpPacketPtr tcpPacket, this->tcpPackets) {
					messageObserver->handleTcpPacket(tcpPacket);
				}
				foreach (UdpPacketPtr udpPacket, this->udpPackets) {
					messageObserver->handleUdpPacket(udpPacket);
				}
			}
		}
		this->tcpPackets.clear();
		this->udpPackets.clear();
		this->packetsMutex->unlock();
		return true;
	}

	void TerrorClient::handleTcpPacket(TcpPacketPtr tcpPacket) {
		int packetLabel = tcpPacket->getLabel();

		if (packetLabel == SERVER_GREETINGS) {
			ServerGreetingsPtr serverGreetings(new ServerGreetings);
			if (!tcpPacket->buildObject(serverGreetings)) {
				return;
			}
		}

		if (packetLabel == REMOVE_PLAYER) {
			RemovePlayerPtr removePlayer(new RemovePlayer);
			tcpPacket->buildObject(removePlayer);

			this->players.erase(removePlayer->getId());
		}

		if (packetLabel == JOIN_INFO) {
			JoinInfoPtr joinInfo(new JoinInfo);
			tcpPacket->buildObject(joinInfo);

			this->playerId = PlayerIdPtr(new PlayerId(joinInfo->getPlayerId()));

			this->possibleHostUuid = joinInfo->getHostUuid();
			this->udpPunchThroughClient->requestPunchThrough(this->possibleHostUuid);

			this->hosting = false;
			this->host.reset();
			this->players.clear();

			int oldHostId = joinInfo->getOldHostId();
			if (oldHostId != -1) {
				RemovePlayerPtr removePlayer(new RemovePlayer(oldHostId));
				TcpPacketPtr auxTcpPacket(new TcpPacket);
				auxTcpPacket->buildPacket(removePlayer);

				this->packetsMutex->try_lock_for(boost::chrono::seconds(60));
				this->tcpPackets.add(auxTcpPacket);
				this->packetsMutex->unlock();
			}
		}	

		if (packetLabel == HOST_INFO) {
			HostInfoPtr hostInfo(new HostInfo);
			tcpPacket->buildObject(hostInfo);				

			if (this->playerId->getId() == -1) {
				this->playerId->setId(0);
			}
			this->hosting = true;
			this->host.reset();
			
			int oldHostId = hostInfo->getOldHostId();
			if (oldHostId != -1) {
				RemovePlayerPtr removePlayer(new RemovePlayer(oldHostId));
				TcpPacketPtr auxTcpPacket(new TcpPacket);
				auxTcpPacket->buildPacket(removePlayer);

				this->packetsMutex->try_lock_for(boost::chrono::seconds(60));
				this->tcpPackets.add(auxTcpPacket);
				this->packetsMutex->unlock();
			}
		}

		this->packetsMutex->try_lock_for(boost::chrono::seconds(60));
		this->tcpPackets.add(tcpPacket);
		this->packetsMutex->unlock();
	}

	void TerrorClient::handleUdpPacket(UdpPacketPtr udpPacket) {
		if (this->playerId->getId() != -1 && !this->hosting && (!this->host || !udpPacket->getRemoteUdpAddress()->equalTo(this->host))) {
			return;
		}

		if (this->hosting) {
			if (udpPacket->getLabel() == PLAYER_ID) {
				PlayerIdPtr playerId(new PlayerId);
				if (!udpPacket->buildObject(playerId)) {
					return;
				}
				this->players[playerId->getId()] = udpPacket->getRemoteUdpAddress();

				JoinAcceptedPtr joinAccepted(new JoinAccepted);
				this->sendObjectTo(joinAccepted, udpPacket->getRemoteUdpAddress(), true);
			}
			bool connectedPlayer = false;
			foreach (UdpAddressIntPtrMap::value_type& entry, this->players) {
				if (udpPacket->getRemoteUdpAddress()->equalTo(entry.second)) {
					connectedPlayer = true;
					break;
				}
			}
			if (!connectedPlayer) {
				return;
			}		
		}

		if (this->playerId->getId() != -1 && this->hosting) {
			bool found = false;
			foreach (UdpAddressIntPtrMap::value_type& entry, this->players) {
				UdpAddressPtr playerUdpAddress = entry.second;
				if (udpPacket->getRemoteUdpAddress()->equalTo(playerUdpAddress)) {
					found = true;
				}
			}
			if (!found) {
				return;
			}
		}

		this->packetsMutex->try_lock_for(boost::chrono::seconds(60));
		this->udpPackets.add(udpPacket);
		this->packetsMutex->unlock();			
	}

	void TerrorClient::notifyUdpPunchThroughSuccess(int uuid, UdpAddressPtr udpAddress) {
		std::cout << "\nConnected to player " << uuid;
		if (!this->hosting && this->possibleHostUuid == uuid) {
			this->host = udpAddress;
			this->sendObjectToHost(this->playerId, true);
		}
	}

	void TerrorClient::notifyUdpPunchThroughFail(int uuid, UdpAddressPtr udpAddress) {
		std::cout << "\nFailed to connect to player " << uuid;
	}
}
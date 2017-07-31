#ifndef TERROR_NETWORK_TERROR_CLIENT_H
#define TERROR_NETWORK_TERROR_CLIENT_H

#include "Packets/Packets.h"
#include "NetworkObserver/NetworkObserver.h"

namespace terror_network {
	typedef PointerList<NetworkObserver> NetworkObserverPtrList;
	typedef PointerList<TcpPacket> TcpPacketPtrList;
	typedef PointerList<UdpPacket> UdpPacketPtrList;

	class TerrorClient : public EventHandler, public TcpPacketObserver, public UdpPacketObserver {
	public:
		TerrorClient(UdpPunchThroughClientPtr udpPunchThroughClient, UdpAddressPtr server);

		void addNetworkObserver(NetworkObserverPtr messageObserver);
		void removeNetworkObserver(NetworkObserverPtr messageObserver);
		int getId();
		bool isHost() const;
		void reset();		

		template<class Type_>
		void sendObjectToServer(SharedPointer<Type_> object, bool assertSend = false) {
			this->sendObjectTo(object, this->server, assertSend);
		}

		template<class Type_>
		void sendObjectToHost(SharedPointer<Type_> object, bool assertSend = false) {
			if (!this->host) {
				return;
			}
			this->sendObjectTo(object, this->host, assertSend);
		}

		template<class Type_>
		void sendObjectToPlayers(SharedPointer<Type_> object, bool assertSend = false) {
			this->sendObjectTo(object, this->players, assertSend);
		}	

		virtual ~TerrorClient();

	protected:
		UdpPunchThroughClientPtr udpPunchThroughClient;

		NetworkObserverPtrList messageObservers;
		TcpPacketPtrList tcpPackets;
		UdpPacketPtrList udpPackets;
		MutexPtr packetsMutex;

		UdpAddressPtr server;
		UdpAddressPtr host;
		UdpAddressIntPtrMap players;		

		PlayerIdPtr playerId;
		bool hosting;
		int possibleHostUuid;

	private:
		virtual bool handleEvent(float diffTime);
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket);
		virtual void handleUdpPacket(UdpPacketPtr udpPacket);

		void notifyUdpPunchThroughSuccess(int uuid, UdpAddressPtr udpAddress);
		void notifyUdpPunchThroughFail(int uuid, UdpAddressPtr udpAddress);

	};

	typedef SharedPointer<TerrorClient> TerrorClientPtr;
}

#endif
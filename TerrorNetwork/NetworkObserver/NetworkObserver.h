#ifndef TERROR_NETWORK_NETWORK_OBSERVER_H
#define TERROR_NETWORK_NETWORK_OBSERVER_H

#include "Config.h"

namespace terror_network {
	class NetworkObserver {
	public:
		NetworkObserver(bool ingameOnly = true) : ingameOnly(ingameOnly) {}

		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {}
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {}

		bool isIngameOnly() {
			return this->ingameOnly;
		}

		virtual ~NetworkObserver() {}

	protected:
		bool ingameOnly;
	};

	typedef SharedPointer<NetworkObserver> NetworkObserverPtr;
}

#endif
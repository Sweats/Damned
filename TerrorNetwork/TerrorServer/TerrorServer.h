#ifndef TERROR_NETWORK_TERROR_SERVER_H
#define TERROR_NETWORK_TERROR_SERVER_H

#include "Packets/Packets.h"

namespace terror_network {
	class TerrorServer : public TcpPacketObserver {
	public:
		TerrorServer(ConnectedUserSystemPtr connectedUserSystem);

		virtual ~TerrorServer();

	protected:
		ConnectedUserSystemPtr connectedUserSystem;
		GameSessionPtrMap gameSessions;		
		GameSessionPtrMap gameSessionsByUserUuid;
		MutexPtr gameSessionsMutex;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket);
		virtual void handleTcpDisconnect();
		void leaveGameSession(int uuid);
	};

	typedef SharedPointer<TerrorServer> TerrorServerPtr;
}

#endif
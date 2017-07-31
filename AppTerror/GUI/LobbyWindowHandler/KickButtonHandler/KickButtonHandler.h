#ifndef TERROR_KICK_BUTTON_HANDLER_H
#define TERROR_KICK_BUTTON_HANDLER_H

#include "Config.h"
#include "KickHandler/KickHandler.h"

namespace terror {
	class KickButtonHandler : public ButtonHandler {
	public:
		KickButtonHandler(Widget* kickButton, TerrorClientPtr terrorClient, KickHandlerPtr kickHandler, int playerId)
			: ButtonHandler(kickButton), kickButton(kickButton), terrorClient(terrorClient), kickHandler(kickHandler), playerId(playerId), buttonVisible(false) {}

		void update() {
			this->kickButton->setVisible(this->buttonVisible);
		}

		void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (!this->terrorClient->isHost()) {
				return;
			}
			if (udpPacket->getLabel() == GREETINGS) {
				GreetingsPtr greetings(new Greetings);
				if (!udpPacket->buildObject(greetings)) {
					return;
				}
				if (greetings->getId() == this->playerId) {
					this->buttonVisible = true;
					this->update();
				}
			}
		}

		void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (this->terrorClient->isHost() && tcpPacket->getLabel() == REMOVE_PLAYER) {
				RemovePlayerPtr removePlayer(new RemovePlayer);
				tcpPacket->buildObject(removePlayer);
				if (removePlayer->getId() == this->playerId) {
					this->buttonVisible = false;
					this->update();
				}
			}
		}

		void reset() {
			this->buttonVisible = false;
			this->update();
		}

		virtual ~KickButtonHandler() {}

	protected:
		TerrorClientPtr terrorClient;
		KickHandlerPtr kickHandler;
		Widget* kickButton;
		int playerId;
		bool buttonVisible;

	private:
		virtual void handleButtonClick() {
			if (this->terrorClient->isHost()) {
				this->kickHandler->kickPlayer(this->playerId);
			}
		}
	};

	typedef SharedPointer<KickButtonHandler> KickButtonHandlerPtr; 
}

#endif
#ifndef TERROR_GAME_SESSION_LISTBOX_HANDLER_H
#define TERROR_GAME_SESSION_LISTBOX_HANDLER_H

#include <iomanip>
#include "Config.h"

namespace terror {
	typedef IntPointerMap<CpuTimer> CpuTimerPtrMap;

	class GameSessionListboxHandler : public ButtonHandler, public NetworkObserver {
	public:
		GameSessionListboxHandler(Widget* refreshButton, MyGUI::MultiListBox* gameSessionsListbox, TerrorClientPtr terrorClient) 
			: ButtonHandler(refreshButton), NetworkObserver(false), gameSessionsListbox(gameSessionsListbox), terrorClient(terrorClient) {}

		virtual ~GameSessionListboxHandler() {}

	protected:
		MyGUI::MultiListBox* gameSessionsListbox;
		TerrorClientPtr terrorClient;
		CpuTimerPtrMap pingTimers;

	private:
		virtual void handleButtonClick() {
			GameSessionListRequestPtr gameSessionListRequest(new GameSessionListRequest);
			this->terrorClient->sendObject(gameSessionListRequest);
		}

		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == GAME_LIST) {
				GameSessionListPtr gameSessionList(new GameSessionList);
				tcpPacket->buildObject(gameSessionList);
				this->populateGameSessionsListbox(gameSessionList);
				this->startGameSessionsPingsMeasurement(gameSessionList);
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == PING_RESPONSE) {
				PingResponsePtr pingResponse(new PingResponse);
				if (!udpPacket->buildObject(pingResponse)) {
					return;
				}

				int pingId = pingResponse->getId();
				CpuTimerPtr pingTimer = this->pingTimers.get(pingId);
				if (pingTimer) {
					int ping = static_cast<int>(pingTimer->elapsed().wall)/1e6;
					ping = std::max(0, ping - 17); 
					pingTimer->stop();
					this->pingTimers.erase(pingId);
					this->gameSessionsListbox->setSubItemNameAt(5, pingId, lexical_cast<string>(ping));
				}
			}
		}

		void populateGameSessionsListbox(GameSessionListPtr gameSessionList) {
			this->gameSessionsListbox->removeAllItems();
			int gameSessionId = 0;
			foreach (GameSessionInfoPtr gameSessionInfo, *gameSessionList) {
				std::stringstream idStream;
				idStream << std::setw(3) << std::setfill('0') << gameSessionInfo->getId();
				this->gameSessionsListbox->addItem(idStream.str());

				this->gameSessionsListbox->setSubItemNameAt(1, gameSessionId, gameSessionInfo->getName());

				string players = lexical_cast<string>(gameSessionInfo->getNumberOfPlayers()) + "/" + lexical_cast<string>(gameSessionInfo->getMaxPlayers());
				this->gameSessionsListbox->setSubItemNameAt(2, gameSessionId, players);

				string publicSession = gameSessionInfo->isPrivate() ? "No" : "Yes";
				this->gameSessionsListbox->setSubItemNameAt(3, gameSessionId, publicSession);				

				string inLobby = gameSessionInfo->getStageName() != "" ? "No" : "Yes";
				this->gameSessionsListbox->setSubItemNameAt(4, gameSessionId, inLobby);

				++gameSessionId;
			}
			this->gameSessionsListbox->sortByColumn(0);
		}

		void startGameSessionsPingsMeasurement(GameSessionListPtr gameSessionList) {
			int gameSessionId = 0;
			foreach (GameSessionInfoPtr gameSessionInfo, *gameSessionList) {
				PingRequestPtr pingRequest(new PingRequest(gameSessionId));
				UserInfoPtr hostInfo = gameSessionInfo->getHostInfo();
				this->pingTimers[gameSessionId] = CpuTimerPtr(new CpuTimer);
				this->pingTimers[gameSessionId]->start();
				if (SteamSystem::getInstance()->isValid() && hostInfo->getSteamUdpAddress()->isValid() ) {
					this->terrorClient->sendObjectTo(pingRequest, hostInfo->getSteamUdpAddress(), true);
				}else {
					this->terrorClient->sendObjectTo(pingRequest, hostInfo->getAsioUdpAddress(), true);
				}
				++gameSessionId;
			}
		}


	};

	typedef SharedPointer<GameSessionListboxHandler> GameSessionListboxHandlerPtr;
}

#endif
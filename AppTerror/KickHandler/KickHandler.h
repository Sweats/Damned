#ifndef TERROR_KICK_HANDLER_H
#define TERROR_KICK_HANDLER_H

#include "StageHandler/StageHandler.h"

namespace terror {
	class KickHandler : public NetworkObserver {
	public:
		KickHandler(GuiLayoutPtr mainLayout, GuiLayoutPtr pauseLayout, GuiLayoutPtr loadingLayout, GuiLayoutPtr lobbyLayout, 
			StageHandlerPtr stageHandler, TerrorClientPtr terrorClient) 
			: mainLayout(mainLayout), pauseLayout(pauseLayout), loadingLayout(loadingLayout), lobbyLayout(lobbyLayout), 
			stageHandler(stageHandler), terrorClient(terrorClient), ingame(false) {}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->ingame = false;
		}

		void kickPlayer(int playerId) {
			if (!this->terrorClient->isHost() || this->terrorClient->getId() == playerId) {
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(playerId);
			if (!playerInfo || playerInfo->getName() == "") {
				return;
			}

			std::string message = std::string("Player ") + playerInfo->getName() + std::string(" got kicked out of this game!");
			writeMessage(message);

			KickPlayerPtr kickPlayer(new KickPlayer(playerId));
			this->terrorClient->sendObjectToPlayers(kickPlayer);
		}

		virtual ~KickHandler() {}

	protected:
		GuiLayoutPtr mainLayout;
		GuiLayoutPtr pauseLayout;
		GuiLayoutPtr loadingLayout;
		GuiLayoutPtr lobbyLayout;
		StageHandlerPtr stageHandler;
		TerrorClientPtr terrorClient;
		bool ingame;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == KICK_PLAYER) {
				KickPlayerPtr kickPlayer(new KickPlayer);
				udpPacket->buildObject(kickPlayer);
				
				if (kickPlayer->playerId != this->terrorClient->getId()) {
					PlayersStatePtr playersState = PlayersState::getInstance();
					PlayerInfoPtr playerInfo = playersState->getPlayerInfo(kickPlayer->playerId);
					std::string message = std::string("Player ") + playerInfo->getName() + std::string(" got kicked out of this game!");
					writeMessage(message);
					return;
				}

				LeaveGameRequestPtr leaveGameRequest(new LeaveGameRequest);
				this->terrorClient->sendObject(leaveGameRequest);
				this->terrorClient->reset();
				if (this->ingame) {
					this->pauseLayout->hide();
					MyGUI::ImageBox* loadingImagebox = static_cast<MyGUI::ImageBox*>(this->loadingLayout->getWidget("LoadingImagebox"));
					loadingImagebox->setImageTexture(std::string("loading.jpg"));
					this->loadingLayout->show(this->mainLayout);
					this->stageHandler->requestUnloadStage();
				}else {
					this->lobbyLayout->hide();
				}
				std::string message = std::string("You got kicked!");
				writeMessage(message);
			}
		}
	};

	typedef SharedPointer<KickHandler> KickHandlerPtr;
}

#endif
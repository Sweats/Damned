#ifndef TERROR_PLAYERS_STATS_HANDLER_H
#define TERROR_PLAYERS_STATS_HANDLER_H

#include "KickHandler/KickHandler.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	typedef IntPointerMap<CpuTimer> CpuTimerPtrMap;

	class PlayersStatsHandler : public NetworkObserver, public InputHandler {
	public:
		PlayersStatsHandler(KickHandlerPtr kickHandler, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave) 
			: kickHandler(kickHandler), playersLayout(loadLayout("Players.layout")), terrorClient(terrorClient), terrorSave(terrorSave), playersPing(new PlayersPing), ingame(false) 
		{
			this->playersLayout->hide(false);
			this->playersListbox = static_cast<MyGUI::MultiListBox*>(this->playersLayout->getWidget("PlayersMultiList"));
		}

		void notifyStageLoaded(StagePtr stage) {
			this->playersLayout->hide(false);
			this->ingame = true;
		}

		void notifyStageUnloaded() {
			this->playersLayout->hide(false);
			this->ingame = false;
		}

		virtual ~PlayersStatsHandler() {}

	protected:
		KickHandlerPtr kickHandler;
		GuiLayoutPtr playersLayout;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		MyGUI::MultiListBox* playersListbox;
		PlayersPingPtr playersPing;
		CpuTimerPtrMap pingTimers;
		bool ingame;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (!this->ingame) {
				return;
			}
			if (button == terrorSave->controls->tab) {
				this->playersLayout->show();

				if (!this->terrorClient->isHost()) {
					PlayersPingRequestPtr playersPingRequest(new PlayersPingRequest);
					this->terrorClient->sendObjectToHost(playersPingRequest, true);
					return;
				}
				this->renewPlayersPing();
			}

			if (!this->terrorClient->isHost()) {
				return;
			}

			if (button == BC_F1) {
				this->kickHandler->kickPlayer(0);
			}
			if (button == BC_F2) {
				this->kickHandler->kickPlayer(1);
			}
			if (button == BC_F3) {
				this->kickHandler->kickPlayer(2);
			}
			if (button == BC_F4) {
				this->kickHandler->kickPlayer(3);
			}
			if (button == BC_F5) {
				this->kickHandler->kickPlayer(4);
			}
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (!this->ingame) {
				return;
			}
			if (button == terrorSave->controls->tab) {
				this->playersLayout->hide(false);
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			// Host
			if (udpPacket->getLabel() == PING_RESPONSE) {
				PingResponsePtr pingResponse(new PingResponse);
				if (!udpPacket->buildObject(pingResponse)) {
					return;
				}

				int pingId = pingResponse->getId();
				CpuTimerPtr pingTimer = this->pingTimers.get(pingId);
				if (pingTimer) {
					int ping = static_cast<int>(pingTimer->elapsed().wall)/1e6;
					ping = std::max(0, ping - 17); /*16.666 is the time consumed by one frame, which is the time 
												   the ping response takes to reach here since its arrival;*/
					(*this->playersPing)[pingId] = ping;
					pingTimer->stop();
					this->pingTimers.erase(pingId);
				}
				if (this->pingTimers.size() == 0 || static_cast<int>(this->pingTimers.begin()->second->elapsed().wall)/1e6 > 1000) {
					foreach (CpuTimerPtrMap::value_type& entry, this->pingTimers) {
						(*this->playersPing)[entry.first] = -1;
					}
					this->pingTimers.clear();
					this->terrorClient->sendObjectToPlayers(this->playersPing, true);
					this->populatePlayersList();
				}
			}

			if (udpPacket->getLabel() == PLAYERS_PING_REQUEST) {
				this->renewPlayersPing();
			}

			// Client
			if (udpPacket->getLabel() == PLAYERS_PING) {
				if (!udpPacket->buildObject(this->playersPing)) {
					return;
				}
				this->populatePlayersList();
			}
		}	

		void renewPlayersPing() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			for (int id = 0; id < 5; ++id) {
				(*this->playersPing)[id] = 0;

				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(id);
				if (playerInfo && playerInfo->getName() != "") {
					if (id != this->terrorClient->getId()) {
						PingRequestPtr pingRequest(new PingRequest(id));
						this->terrorClient->sendObjectTo(pingRequest, playerInfo->getUdpAddress(), true);
						this->pingTimers[id] = CpuTimerPtr(new CpuTimer);
						this->pingTimers[id]->start();
					}
				}
			}
			if (this->pingTimers.size() == 0) {
				this->terrorClient->sendObjectToPlayers(this->playersPing, true);
				this->populatePlayersList();
			}
		}

		void populatePlayersList() {
			this->playersListbox->removeAllItems();
			PlayersStatePtr playersState = PlayersState::getInstance();
			int itemId = 0;
			for (int id = 0; id < 5; ++id) {
				PlayerInfoPtr playerInfo = playersState->getPlayerInfo(id);
				if (playerInfo && playerInfo->getName() != "") {
					string playerName;
					if (this->terrorClient->isHost() && this->terrorClient->getId() != id) {
						playerName = string("(F") + lexical_cast<string>(id + 1) + string(" to Kick) ");
					}
					if (this->terrorClient->getId() != id) {
						playerName += string("(F") + lexical_cast<string>(id + 6) + string(" to Mute/Unmute) ");
					}
					playerName += playerInfo->getName();
					this->playersListbox->addItem(playerName);
					string characterName = this->getCharacterString(playerInfo->getCharacter()) + 
						(playersState->isCharacterDead(playerInfo->getCharacter()) ? string(" (Dead)") : string(""));
					this->playersListbox->setSubItemNameAt(1, itemId, characterName);
					string ping = (*this->playersPing)[id] >= 0 ? lexical_cast<string>((*this->playersPing)[id]) : "~";
					this->playersListbox->setSubItemNameAt(2, itemId, ping);
					++itemId;
				}
			}
		}

		string getCharacterString(CharacterType character) {
			switch (character) {
			case CHAR_0:
				return "Mia";
				break;
			case CHAR_1:
				return "Scott";
				break;
			case CHAR_2:
				return "Jacob";
				break;
			case CHAR_3:
				return "Father John";
				break;
			case CHAR_MONSTER_0:
			case CHAR_MONSTER_1:
			case CHAR_MONSTER_2:
			case CHAR_MONSTER_3:
				return "Monster";
				break;
			case CHAR_RANDOM:
			case CHAR_NONE:
				return "";
				break;
			}
			return "";
		}
	};

	typedef SharedPointer<PlayersStatsHandler> PlayersStatsHandlerPtr;
}

#endif
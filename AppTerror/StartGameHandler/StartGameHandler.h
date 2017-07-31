#ifndef TERROR_START_GAME_HANDLER_H
#define TERROR_START_GAME_HANDLER_H

#include "PauseHandler/PauseHandler.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	class StartGameHandler : public NetworkObserver, public InputHandler {
	public:
		StartGameHandler(GuiLayoutPtr startGameLayout, GuiLayoutPtr pauseLayout, SoundPtr startGameSound, PauseHandlerPtr pauseHandler, TerrorClientPtr terrorClient, TerrorSavePtr terrorSave, ButtonTranslatorPtr buttonTranslator) 
			: NetworkObserver(false), startGameLayout(startGameLayout), pauseLayout(pauseLayout), startGameSound(startGameSound), pauseHandler(pauseHandler), terrorClient(terrorClient), terrorSave(terrorSave),
			buttonTranslator(buttonTranslator),	hasToStart(false), started(false) 
		{
			this->messageTextbox = static_cast<MyGUI::TextBox*>(startGameLayout->getWidget("MessageTextbox"));
			this->startGameLayout->connectShowEvent(Bind(&StartGameHandler::hideStartGameLayoutIfNotPregame, this));
		}

		void notifyStageLoaded(StagePtr stage) {
			this->startGameLayout->show();
			if (this->terrorSave->controls->start == BC_UNASSIGNED) {
				this->terrorSave->controls->start = BC_RETURN;
				this->terrorSave->save();
			}
			if (this->terrorClient->isHost()) {			
				this->messageTextbox->setCaption("Wait for everybody to connect, then press " + this->buttonTranslator->translateButton(this->terrorSave->controls->start) + " to start the game!");
			}else {
				this->messageTextbox->setCaption("Wait for the host to start the game...");
			}
			this->hasToStart = true;
			this->started = false;
			this->pauseHandler->pause(true);
		}

		void notifyStageUnloaded() {
			this->hasToStart = false;
			this->started = false;
			this->pauseHandler->setPausable(false);
			this->pauseHandler->resume();
			this->startGameLayout->hide();
		}

		virtual ~StartGameHandler() {}

	protected:
		GuiLayoutPtr startGameLayout;
		GuiLayoutPtr pauseLayout;
		MyGUI::TextBox* messageTextbox;
		SoundPtr startGameSound;
		PauseHandlerPtr pauseHandler;
		TerrorClientPtr terrorClient;
		TerrorSavePtr terrorSave;
		ButtonTranslatorPtr buttonTranslator;
		bool hasToStart;
		bool started;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (!this->hasToStart || !this->terrorClient->isHost()) {
				return;
			}
			if (button == this->terrorSave->controls->start || button == BC_NUMPADENTER) {
				this->startGame();
				writeMessage("You started the game!");

				StartGamePtr startGame(new StartGame);
				this->terrorClient->sendObjectToPlayers(startGame, true);
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == START_GAME) {
				if (this->started) {
					return;
				}
				this->startGame();
				writeMessage("The host started the game!");
			}

			if (udpPacket->getLabel() == PLAYER_JOINED_GAME) {
				PlayerJoinedGamePtr playerJoinedGame(new PlayerJoinedGame);
				if (!udpPacket->buildObject(playerJoinedGame)) {
					return;
				}

				if (playerJoinedGame->getPlayerId() != -1) {
					PlayersStatePtr playersState = PlayersState::getInstance();
					PlayerInfoPtr playerInfo = playersState->getPlayerInfo(playerJoinedGame->getPlayerId());
					if (playerInfo) {
						std::string playerName = playerInfo->getName();
						std::string message = std::string("Player ") + playerName + std::string(" joined the game!");
						writeMessage(message);
						if (this->terrorClient->isHost()) {
							this->terrorClient->sendObjectToPlayers(playerJoinedGame, true);
						}
					}
				}			

				if (!this->started) {
					return;
				}
				StartGamePtr startGame(new StartGame);
				this->terrorClient->sendObjectTo(startGame, udpPacket->getRemoteUdpAddress(), true);
			}
		}

		void startGame() {
			this->startGameLayout->hide();
			this->hasToStart = false;
			this->started = true;
			this->pauseHandler->setPausable(true);
			if (this->pauseLayout->isVisible()) {
				Core::getInstance()->switchState("Pause");
			}else {
				this->pauseHandler->resume();
			}
			this->startGameSound->play();
		}

		void hideStartGameLayoutIfNotPregame() {
			if (this->startGameLayout->isVisible() && !(Core::getInstance()->getCurrentStateName() == "Pregame")) {
				this->startGameLayout->hide();
			}
		}
	};
	typedef SharedPointer<StartGameHandler> StartGameHandlerPtr;
}

#endif
#ifndef TERROR_CHAT_WINDOW_HANDLER_H
#define TERROR_CHAT_WINDOW_HANDLER_H

#include "PauseHandler/PauseHandler.h"
#include "Packets/PlayersState/PlayersState.h"

namespace terror {
	class ChatWindowHandler : public NetworkObserver, public InputHandler {
	public:
		ChatWindowHandler(GuiLayoutPtr chatLayout, TerrorClientPtr terrorClient, PauseHandlerPtr pauseHandler, TerrorSavePtr terrorSave)
			: NetworkObserver(false), chatLayout(chatLayout), terrorClient(terrorClient), pauseHandler(pauseHandler), terrorSave(terrorSave), mustHide(false), ingame(false), writing(false), timeToIgnore(-1.0f)
		{
			this->chatEditbox = static_cast<MyGUI::EditBox*>(chatLayout->getWidget("ChatEditbox"));
			this->chatWindow = static_cast<MyGUI::TextBox*>(chatLayout->getWidget("ChatWindow"));

			this->chatEditbox->eventEditTextChange += MyGUI::newDelegate(this, &ChatWindowHandler::handleTextChange);
			this->chatEditbox->eventEditSelectAccept += MyGUI::newDelegate(this, &ChatWindowHandler::handleTextAccept);
		}

		void reset() {
			this->chatMessages.clear();
			this->chatWindow->setCaption("");
		}

		void notifyStageLoaded(StagePtr stage) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (playerInfo && !isMonsterCharacter(playerInfo->getCharacter())) {
				this->ingame = true;
			}
			this->timeToIgnore = -1.0f;
		}

		void notifyStageUnloaded() {
			this->ingame = false;
			this->mustHide = false;
			this->writing = false;
			this->timeToIgnore = -1.0f;
		}

		virtual ~ChatWindowHandler() {}

	protected:
		GuiLayoutPtr chatLayout;
		TerrorClientPtr terrorClient;
		PauseHandlerPtr pauseHandler;
		TerrorSavePtr terrorSave;
		MyGUI::EditBox* chatEditbox;
		MyGUI::TextBox* chatWindow;
		StringList chatMessages;
		float timeToHide;
		float timeToIgnore;
		bool mustHide;
		bool ingame;
		bool writing;

	private:
		virtual bool handleInput(float diffTime) {
			if (this->terrorClient->getId() == -1) {
				return true;
			}

			if (!this->ingame) {
				return true;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (playerInfo && isMonsterCharacter(playerInfo->getCharacter())) {
				this->chatLayout->hide();
				this->mustHide = false;
				this->timeToHide = 0.0f;
				this->ingame = false;
			}

			if (Core::getInstance()->getCurrentStateName() == "Pregame" && this->terrorClient->isHost()) {
				this->timeToIgnore = 1.0f;
			}

			if (this->timeToIgnore > 0.0f) {
				this->timeToIgnore -= diffTime;
			}

			if (this->writing || !this->mustHide) {
				return true;
			}

			this->timeToHide -= diffTime;
			if (this->timeToHide <= 0.0f) {
				this->chatLayout->hide();
				this->mustHide = false;		
			}
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (!this->ingame || this->timeToIgnore > 0.0f) {
				return;
			}
			if (button == this->terrorSave->controls->start) {
				if (InputManager::getInstancePtr()->getKeyFocusWidget() != this->chatEditbox) {
					if (this->pauseHandler->isPaused()) {
						return;
					}
					InputManager::getInstancePtr()->setKeyFocusWidget(this->chatEditbox);
					this->chatEditbox->setCaption("");
					this->pauseHandler->pause();
					this->chatLayout->show();
					this->mustHide = false;
					this->writing = true;
					this->timeToHide = 0.0f;
				}else {
					InputManager::getInstancePtr()->resetKeyFocusWidget();
					this->pauseHandler->resume();
					this->mustHide = true;
					this->writing = false;
					this->timeToHide = 5.0f;
				}
			}
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == CHAT_MESSAGE) {				
				if (this->ingame && !this->writing) {
					this->chatLayout->show();
					this->mustHide = true;
					this->timeToHide = 5.0f;
				}

				ChatMessagePtr chatMessage(new ChatMessage);
				if (!udpPacket->buildObject(chatMessage)) {
					return;
				}

				string message = chatMessage->getMessage();
				this->writeMessageInChatWindow(message);
				if (this->terrorClient->isHost()) {
					this->terrorClient->sendObjectToPlayers(chatMessage, true);
				}
			}
		}

		void handleTextChange(Widget* chatEditbox) {
			string message = this->chatEditbox->getOnlyText();
			erase_all(message, "#");
			int maxCharPerMessage = 200;
			if (message.size() > maxCharPerMessage) {
				message = message.substr(0, maxCharPerMessage);				
			}
			this->chatEditbox->setCaption(message);
		}

		void handleTextAccept(Widget* chatEditbox) {
			if (this->chatEditbox->getOnlyText() == "") {				
				return;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			string message = this->getChatColor() + playerInfo->getName() + string(": #cccccc");
			message += this->chatEditbox->getOnlyText();
			ChatMessagePtr chatMessage(new ChatMessage(message));

			if (this->terrorClient->isHost()) {
				this->writeMessageInChatWindow(message);
				this->terrorClient->sendObjectToPlayers(chatMessage, true);
			}else {
				this->terrorClient->sendObjectToHost(chatMessage, true);
			}
			this->chatEditbox->setCaption("");
		}

		void writeMessageInChatWindow(const string& message) {
			int charsPerLine = 95;
			int nSplits = message.size()/charsPerLine;			
			for (int i = 0; i < nSplits; ++i) {
				this->chatMessages.push_back(message.substr(charsPerLine*i, charsPerLine));
			}
			this->chatMessages.push_back(message.substr(charsPerLine*nSplits, charsPerLine));

			int nLines = 9;
			while (this->chatMessages.size() > nLines) {
				this->chatMessages.pop_front();
			}
			
			string finalMessage;
			foreach (string& chatMessage, this->chatMessages) {
				finalMessage += chatMessage + string("\n");
			}

			this->chatWindow->setCaption(finalMessage);
		}

		string getChatColor() {
			switch (this->terrorClient->getId()) {
				case 0:
					return "#0088ff";
					break;
				case 1:
					return "#ff0000";
					break;
				case 2:
					return "#00ff00";
					break;
				case 3:
					return "#ff8800";
					break;
				case 4:
					return "#aaaa00";
					break;				
				default:
					return "#cccccc";
			}
		}
	};

	typedef SharedPointer<ChatWindowHandler> ChatWindowHandlerPtr;
}

#endif
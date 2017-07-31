#ifndef TERROR_MESSAGE_SYSTEM_H
#define TERROR_MESSAGE_SYSTEM_H

#include "Stage/Stage.h"

namespace terror {
	class Message {
	public:
		Message(const std::string& messageText) : timeToShow(10.0f)
		{
			int charsPerLine = 80;
			int nSplits = messageText.size()/charsPerLine;		
			for (int i = 0; i < nSplits; ++i) {
				this->textLines.push_back(messageText.substr(charsPerLine*i, charsPerLine));
			}
			this->textLines.push_back(messageText.substr(charsPerLine*nSplits, charsPerLine));
		}

		StringList& getTextLines() {
			return this->textLines;
		}

		void update(float diffTime) {
			this->timeToShow -= diffTime;
		}

		bool keepShowing() {
			return this->timeToShow > 0.0f;
		}

		virtual ~Message() {}

	protected:
		StringList textLines;
		float timeToShow;
	};
	typedef SharedPointer<Message> MessagePtr;

	typedef PointerList<Message> MessagePtrList;
	class MessageSystem : public EventHandler {
	public:
		typedef SharedPointer<MessageSystem> MessageSystemPtr;

		static MessageSystemPtr getInstance() {
			if (!MessageSystem::instance) {
				MessageSystem::instance = MessageSystemPtr(new MessageSystem);
			}
			return MessageSystem::instance;
		}

		void writeMessage(const string& newMessage) {
			MessagePtr message(new Message(newMessage));
			this->messages.add(message);
		}

		virtual ~MessageSystem() {}

	protected:
		GuiLayoutPtr messageLayout;
		MyGUI::TextBox* messageTextbox;
		MessagePtrList messages;

	private:
		static MessageSystemPtr instance;

		MessageSystem() : EventHandler(20), messageLayout(loadLayout("LogMessage.layout")) {
			this->messageTextbox = static_cast<MyGUI::TextBox*>(this->messageLayout->getWidget("MessageTextbox"));
			this->messageLayout->show();
		}

		virtual bool handleEvent(float diffTime) {
			MessagePtrList messagesSafeCopy = this->messages;
			foreach (MessagePtr message, messagesSafeCopy) {
				message->update(diffTime);
				if (!message->keepShowing()) {
					this->messages.erase(message);
				}
			}

			this->messageTextbox->setCaption(this->getBuildText());			
			return true;
		}

		std::string getBuildText() {			
			int counter = 0;
			std::string finalText = "";
			reverse_foreach (MessagePtr message, this->messages) {
				reverse_foreach (std::string text, message->getTextLines()) {					
					++counter;
					finalText = text + "\n" + finalText;
					if (counter >= 4) {
						return finalText;
					}
				}
			}
			return finalText;
		}
	};

	typedef MessageSystem::MessageSystemPtr MessageSystemPtr;

	MessageSystemPtr MessageSystem::instance = MessageSystemPtr();

	void writeMessage(const std::string& newMessage) {
		MessageSystem::getInstance()->writeMessage(newMessage);
	}
}

#endif
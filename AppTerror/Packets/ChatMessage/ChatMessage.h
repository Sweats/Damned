#ifndef TERROR_CHAT_MESSAGE_H
#define TERROR_CHAT_MESSAGE_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class ChatMessage : public Sendable {
	public:
		friend class boost::serialization::access;

		ChatMessage() {}

		ChatMessage(const string& message) : message(message) {}

		virtual int getLabel() const {
			return CHAT_MESSAGE;
		}

		string getMessage() {
			return this->message;
		}

		virtual ~ChatMessage() {}

	protected:
		string message;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->message;
		}
	};

	typedef SharedPointer<ChatMessage> ChatMessagePtr;
}

#endif
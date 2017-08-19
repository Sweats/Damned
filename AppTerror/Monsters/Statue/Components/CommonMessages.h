#ifndef APP_TERROR_COMMON_MESSAGES_H
#define APP_TERROR_COMMON_MESSAGES_H

#include "Human/Human.h"

namespace terror {
	class TrapArmedMessage : public entities::Message {
	public:
		TrapArmedMessage() {}

		virtual ~TrapArmedMessage() {}
	};
	typedef SharedPointer<TrapArmedMessage> TrapArmedMessagePtr;

	class HumanAddedMessage : public entities::Message {
	public:
		HumanAddedMessage(HumanPtr human) : human(human) {}

		HumanPtr getAddedHuman() {
			return this->human;
		}

		virtual ~HumanAddedMessage() {}

	private:
		HumanPtr human;
	};
	typedef SharedPointer<HumanAddedMessage> HumanAddedMessagePtr;

	enum PovType {
		FIRST_PERSON,
		THIRD_PERSON
	};
	class PovChangedMessage : public entities::Message {
	public:
		PovChangedMessage(PovType currentPov) : currentPov(currentPov) {}

		PovType getCurrentPov() {
			return this->currentPov;
		}

		virtual ~PovChangedMessage() {}

	private:
		PovType currentPov;
	};
	typedef SharedPointer<PovChangedMessage> PovChangedMessagePtr;

	enum StatueState {
		TRAPPER,
		SEEKER,
		DESTROYER
	};
	class StateChangedMessage : public entities::Message {
	public:
		StateChangedMessage(StatueState currentState) : currentState(currentState) {}

		StatueState getCurrentState() {
			return this->currentState;
		}

		virtual ~StateChangedMessage() {}

	private:
		StatueState currentState;
	};
	typedef SharedPointer<StateChangedMessage> StateChangedMessagePtr;

	class DestroyerEndedMessage : public entities::Message {
	public:
		DestroyerEndedMessage() {}

		virtual ~DestroyerEndedMessage() {}
	};
	typedef SharedPointer<DestroyerEndedMessage> DestroyerEndedMessagePtr;
}

#endif
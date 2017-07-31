#ifndef TERROR_NETWORK_GAME_CREATION_REQUEST_H
#define TERROR_NETWORK_GAME_CREATION_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class GameCreationRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		GameCreationRequest() {}

		GameCreationRequest(const string& name, const string& password, int nPrivateSlots, int maxPlayers) 
			: name(name), password(password), nPrivateSlots(nPrivateSlots), maxPlayers(maxPlayers) {}

		virtual int getLabel() const {
			return GAME_CREATION_REQUEST;
		}

		string getName() const {
			return this->name;
		}

		string getPassword() const {
			return this->password;
		}

		int getNumberOfPrivateSlots() const {
			return this->nPrivateSlots;
		}

		int getMaxPlayers() const {
			return this->maxPlayers;
		}

		virtual ~GameCreationRequest() {}

	protected:
		string name;
		string password;
		int nPrivateSlots;
		int maxPlayers;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
			archive & this->password;
			archive & this->nPrivateSlots;
			archive & this->maxPlayers;
		}
	};

	typedef SharedPointer<GameCreationRequest> GameCreationRequestPtr;
}

#endif
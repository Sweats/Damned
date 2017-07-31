#ifndef TERROR_NETWORK_JOIN_GAME_REQUEST_H
#define TERROR_NETWORK_JOIN_GAME_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class JoinGameRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		JoinGameRequest() {}

		JoinGameRequest(int id, const string& password) 
			: id(id), password(password) {}

		virtual int getLabel() const {
			return JOIN_GAME_REQUEST;
		}

		int getId() const {
			return this->id;
		}

		string getPassword() const {
			return this->password;
		}

		virtual ~JoinGameRequest() {}

	protected:
		int id;
		string password;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
			archive & this->password;
		}
	};

	typedef SharedPointer<JoinGameRequest> JoinGameRequestPtr;
}

#endif
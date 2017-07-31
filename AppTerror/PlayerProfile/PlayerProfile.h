#ifndef TERROR_PLAYER_PROFILE_H
#define TERROR_PLAYER_PROFILE_H

#include "Config.h"

namespace terror {
	class PlayerProfile {
	public:
		PlayerProfile() {}

		void setUsername(const string& username) {
			this->username = username;
		}

		string getUsername() {
			return this->username;
		}

		void setEmail(const string& email) {
			this->email = email;
		}

		string getEmail() {
			return this->email;
		}

		void reset() {
			this->username = "";
			this->email = "";
		}

		virtual ~PlayerProfile() {}

	protected:
		string username;
		string email;
	};

	typedef SharedPointer<PlayerProfile> PlayerProfilePtr;
}

#endif
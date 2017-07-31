#ifndef TERROR_NETWORK_GAME_SESSION_STAGE_NAME_H
#define TERROR_NETWORK_GAME_SESSION_STAGE_NAME_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class GameSessionStageName : public Sendable {
	public:
		friend class boost::serialization::access;

		GameSessionStageName() {}

		GameSessionStageName(const string& stageName) 
			: stageName(stageName) {}

		virtual int getLabel() const {
			return GAME_SESSION_STAGE_NAME;
		}

		string getStageName() const {
			return this->stageName;
		}

		virtual ~GameSessionStageName() {}

	protected:
		string stageName;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->stageName;
		}
	};

	typedef SharedPointer<GameSessionStageName> GameSessionStageNamePtr;
}

#endif
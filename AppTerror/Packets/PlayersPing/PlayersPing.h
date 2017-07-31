#ifndef TERROR_PLAYERS_PING_H
#define TERROR_PLAYERS_PING_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class PlayersPing : public Sendable, public std::vector<int> {
	public:
		friend class boost::serialization::access;

		PlayersPing() {
			this->resize(5, 0);
		}
	
		virtual int getLabel() const {
			return PLAYERS_PING;
		}

		virtual ~PlayersPing() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & BaseObject<std::vector<int> >(*this);
		}
	};

	typedef SharedPointer<PlayersPing> PlayersPingPtr;
}

#endif
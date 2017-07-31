#ifndef TERROR_NETWORK_PLAYER_ID_H
#define TERROR_NETWORK_PLAYER_ID_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	class PlayerId : public Sendable {
	public:
		friend class boost::serialization::access;

		PlayerId() : id(0) {}

		PlayerId(int id) 
			: id(id) {}

		virtual int getLabel() const {
			return PLAYER_ID;
		}

		int getId() const {
			return this->id;
		}

		void setId(int id) {
			this->id = id;
		}

		virtual ~PlayerId() {}

	protected:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<PlayerId> PlayerIdPtr;
}

#endif
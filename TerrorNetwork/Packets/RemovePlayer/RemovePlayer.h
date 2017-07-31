#ifndef TERROR_NETWORK_REMOVE_PLAYER_H
#define TERROR_NETWORK_REMOVE_PLAYER_H

#include "Packets/PacketsEnum.h"

namespace terror_network{
	class RemovePlayer : public Sendable {
	public:
		friend class boost::serialization::access;

		RemovePlayer() {}

		RemovePlayer(int id) : id(id) {}

		virtual int getLabel() const {
			return REMOVE_PLAYER;
		}

		int getId() const {
			return this->id;
		}

		virtual ~RemovePlayer() {}

	protected:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<RemovePlayer> RemovePlayerPtr;
}

#endif
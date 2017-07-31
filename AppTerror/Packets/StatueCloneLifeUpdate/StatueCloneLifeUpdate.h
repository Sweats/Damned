#ifndef TERROR_STATUE_CLONE_LIFE_UPDATE_H
#define TERROR_STATUE_CLONE_LIFE_UPDATE_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class StatueCloneLifeUpdate : public Sendable {
	public:
		friend class boost::serialization::access;

		StatueCloneLifeUpdate() {}

		StatueCloneLifeUpdate(int id, float life) : id(id), life(life) {}

		int getId() {
			return this->id;
		}

		float getLife() {
			return this->life;
		}

		virtual int getLabel() const {
			return STATUE_CLONE_LIFE_UPDATE;
		}

		virtual ~StatueCloneLifeUpdate() {}

	private:
		int id;
		float life;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
			archive & this->life;
		}
	};

	typedef SharedPointer<StatueCloneLifeUpdate> StatueCloneLifeUpdatePtr;
}

#endif
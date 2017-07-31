#ifndef TERROR_SPAWN_STATUE_CLONES_H
#define TERROR_SPAWN_STATUE_CLONES_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct StatueCloneData {
		int id;
		Vector3d position;
		Quaternion orientation;
		float timePosition;

		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
			archive & this->position;
			archive & this->orientation;
			archive & this->timePosition;
		}
	};

	class SpawnStatueClones : public Sendable {
	public:
		friend class boost::serialization::access;

		SpawnStatueClones() {}

		void addStatueCloneData(const StatueCloneData& data) {
			this->datas.push_back(data);
		}

		std::list<StatueCloneData>& getStatueCloneDatas() {
			return this->datas;
		}

		virtual int getLabel() const {
			return SPAWN_STATUE_CLONES;
		}

		virtual ~SpawnStatueClones() {}

	private:
		std::list<StatueCloneData> datas;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->datas;
		}
	};

	typedef SharedPointer<SpawnStatueClones> SpawnStatueClonesPtr;
}

#endif
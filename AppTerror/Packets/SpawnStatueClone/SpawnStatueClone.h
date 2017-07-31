#ifndef TERROR_SPAWN_STATUE_CLONE_H
#define TERROR_SPAWN_STATUE_CLONE_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class SpawnStatueClone : public Sendable {
	public:
		friend class boost::serialization::access;

		SpawnStatueClone() {}

		SpawnStatueClone(int id, const Vector3d& position, const Quaternion& orientation, float timePosition) 
			: id(id), position(position), orientation(orientation), timePosition(timePosition) {}

		int getId() {
			return this->id;
		}

		Vector3d getPosition() {
			return this->position;
		}

		Quaternion getOrientation() {
			return this->orientation;
		}

		float getTimePosition() {
			return this->timePosition;
		}

		virtual int getLabel() const {
			return SPAWN_STATUE_CLONE;
		}

		virtual ~SpawnStatueClone() {}

	private:
		int id;
		Vector3d position;
		Quaternion orientation;
		float timePosition;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
			archive & this->position;
			archive & this->orientation;
			archive & this->timePosition;
		}
	};

	typedef SharedPointer<SpawnStatueClone> SpawnStatueClonePtr;
}

#endif
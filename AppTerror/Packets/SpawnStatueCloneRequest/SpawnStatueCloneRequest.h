#ifndef TERROR_SPAWN_STATUE_CLONE_REQUEST_H
#define TERROR_SPAWN_STATUE_CLONE_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class SpawnStatueCloneRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		SpawnStatueCloneRequest() {}

		SpawnStatueCloneRequest(const Vector3d& position, const Quaternion& orientation, float timePosition) 
			: position(position), orientation(orientation), timePosition(timePosition) {}
	
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
			return SPAWN_STATUE_CLONE_REQUEST;
		}

		virtual ~SpawnStatueCloneRequest() {}

	private:
		Vector3d position;
		Quaternion orientation;
		float timePosition;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->position;
			archive & this->orientation;
			archive & this->timePosition;
		}
	};

	typedef SharedPointer<SpawnStatueCloneRequest> SpawnStatueCloneRequestPtr;
}

#endif
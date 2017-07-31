#ifndef TERROR_OBJECT_INFO_H
#define TERROR_OBJECT_INFO_H

#include "Packets/Vector3dInfo/Vector3dInfo.h"
#include "Packets/QuaternionInfo/QuaternionInfo.h"

namespace terror {
	struct ObjectInfo {
		friend class boost::serialization::access;

		ObjectInfo() {}

		ObjectInfo(ObjectPtr object) : position(object->getPosition()), orientation(object->getOrientation()), id(object->getId()) {}

		virtual ~ObjectInfo() {}

		Vector3dInfo position;
		QuaternionInfo orientation;
		int id;	

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {			
			archive & this->position;
			archive & this->orientation;
			archive & this->id;
		}
	};

	typedef SharedPointer<ObjectInfo> ObjectInfoPtr;
}

#endif
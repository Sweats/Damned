#ifndef TERROR_VECTOR_3D_INFO_H
#define TERROR_VECTOR_3D_INFO_H

#include "Config.h"

namespace terror {
	class Vector3dInfo {
	public:
		friend class boost::serialization::access;

		Vector3dInfo() {}

		Vector3dInfo(const Vector3d& vector) {
			this->setVector3d(vector);
		}

		Vector3dInfo(const Vector3dInfo& vectorInfo) : x(vectorInfo.x), y(vectorInfo.y), z(vectorInfo.z) {}

		Vector3d buildVector3d() const {
			return Vector3d(this->x, this->y, this->z);
		}

		void setVector3d(const Vector3d& vector) {
			this->x = vector.getX();
			this->y = vector.getY();
			this->z = vector.getZ();
		}

		virtual ~Vector3dInfo() {}

	protected:
		float x;
		float y;
		float z;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->x;
			archive & this->y;
			archive & this->z;
		}
	};

	typedef SharedPointer<Vector3dInfo> Vector3dInfoPtr;
}

#endif
#ifndef TERROR_QUATERNION_INFO_H
#define TERROR_QUATERNION_INFO_H

#include "Config.h"

namespace terror {
	class QuaternionInfo {
	public:
		friend class boost::serialization::access;

		QuaternionInfo() {}

		QuaternionInfo(const Quaternion& quaternion) {
			this->setQuaternion(quaternion);
		}

		QuaternionInfo(const QuaternionInfo& quaternionInfo) : w(quaternionInfo.w), x(quaternionInfo.x), y(quaternionInfo.y), z(quaternionInfo.z) {}

		Quaternion buildQuaternion() const {
			return Quaternion(this->w, this->x, this->y, this->z);
		}

		void setQuaternion(const Quaternion& quaternion) {
			this->w = quaternion.getW();
			this->x = quaternion.getX();
			this->y = quaternion.getY();
			this->z = quaternion.getZ();
		}

		virtual ~QuaternionInfo() {}

	protected:
		float w;
		float x;
		float y;
		float z;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->w;
			archive & this->x;
			archive & this->y;
			archive & this->z;
		}
	};

	typedef SharedPointer<QuaternionInfo> QuaternionInfoPtr;
}

#endif
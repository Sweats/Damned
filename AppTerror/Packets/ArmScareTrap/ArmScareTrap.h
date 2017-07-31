#ifndef TERROR_ARM_SCARE_TRAP_H
#define TERROR_ARM_SCARE_TRAP_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class ArmScareTrap : public Sendable {
	public:
		friend class boost::serialization::access;

		ArmScareTrap() {}
		ArmScareTrap(int id) : id(id) {}

		virtual int getLabel() const {
			return ARM_SCARE_TRAP;
		}

		int getId() {
			return this->id;
		}

		virtual ~ArmScareTrap() {}

	protected:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<ArmScareTrap> ArmScareTrapPtr;
}

#endif
#ifndef TERROR_ARM_SCARE_TRAP_REQUEST_H
#define TERROR_ARM_SCARE_TRAP_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class ArmScareTrapRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		ArmScareTrapRequest() {}
		ArmScareTrapRequest(int id) : id(id) {}

		virtual int getLabel() const {
			return ARM_SCARE_TRAP_REQUEST;
		}

		int getId() {
			return this->id;
		}

		virtual ~ArmScareTrapRequest() {}

	protected:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<ArmScareTrapRequest> ArmScareTrapRequestPtr;
}

#endif
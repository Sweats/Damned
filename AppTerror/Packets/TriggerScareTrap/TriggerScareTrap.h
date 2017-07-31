#ifndef TERROR_TRIGGER_SCARE_TRAP_H
#define TERROR_TRIGGER_SCARE_TRAP_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class TriggerScareTrap : public Sendable {
	public:
		friend class boost::serialization::access;

		TriggerScareTrap() {}
		TriggerScareTrap(int id) : id(id) {}

		virtual int getLabel() const {
			return TRIGGER_SCARE_TRAP;
		}

		int getId() {
			return this->id;
		}

		virtual ~TriggerScareTrap() {}

	protected:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<TriggerScareTrap> TriggerScareTrapPtr;
}

#endif
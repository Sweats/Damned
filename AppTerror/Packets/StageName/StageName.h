#ifndef TERROR_STAGE_NAME_H
#define TERROR_STAGE_NAME_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct StageName : public Sendable {
		friend class boost::serialization::access;
		
		StageName() : name("") {}
		StageName(const string& name) : name(name) {}

		virtual int getLabel() const {
			return STAGE_NAME;
		}

		virtual ~StageName() {}

		string name;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
		}
	};

	typedef SharedPointer<StageName> StageNamePtr;
}

#endif
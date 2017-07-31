#ifndef TERROR_STAGE_INFO_H
#define TERROR_STAGE_INFO_H

#include "Packets/PacketsEnum.h"

namespace terror {
	struct StageInfo : public Sendable {
		friend class boost::serialization::access;
		
		StageInfo() : name(""), seed(-1) {}
		StageInfo(const string& name, int seed) : name(name), seed(seed) {}

		virtual int getLabel() const {
			return STAGE_INFO;
		}

		virtual ~StageInfo() {}

		string name;
		int seed;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
			archive & this->seed;
		}
	};

	typedef SharedPointer<StageInfo> StageInfoPtr;
}

#endif
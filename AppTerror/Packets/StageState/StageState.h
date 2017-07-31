#ifndef TERROR_STAGE_STATE_H
#define TERROR_STAGE_STATE_H

#include "Packets/PacketsEnum.h"
#include "Packets/ObjectInfo/ObjectInfo.h"

namespace terror {
	typedef PointerList<ObjectInfo> ObjectInfoPtrList;

	struct StageState : public Sendable {
		friend class boost::serialization::access;
		
		StageState() {}

		virtual int getLabel() const {
			return STAGE_STATE;
		}

		virtual ~StageState() {}

		ObjectInfoPtrList objectInfos;
		IntList destroyedObjectIds;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->objectInfos;
			archive & this->destroyedObjectIds;
		}
	};

	typedef SharedPointer<StageState> StageStatePtr;
}

#endif
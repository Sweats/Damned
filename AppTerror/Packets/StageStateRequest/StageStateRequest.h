#ifndef TERROR_STAGE_STATE_REQUEST_H
#define TERROR_STAGE_STATE_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class StageStateRequest : public Sendable {
	public:
		friend class boost::serialization::access;
		
		StageStateRequest() {}

		virtual int getLabel() const {
			return STAGE_STATE_REQUEST;
		}

		virtual ~StageStateRequest() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<StageStateRequest> StageStateRequestPtr;
}

#endif
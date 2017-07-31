#ifndef TERROR_GRAB_OBJECT_REQUEST_H
#define TERROR_GRAB_OBJECT_REQUEST_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class GrabObjectRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		GrabObjectRequest() {}
		GrabObjectRequest(CharacterType character, int objectId, const Vector3d& handPosition)
			: character(character), objectId(objectId), handPosition(handPosition) {}

		virtual int getLabel() const {
			return GRAB_OBJECT_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getObjectId() {
			return this->objectId;
		}
		
		Vector3d getHandPosition() {
			return this->handPosition.buildVector3d();
		}

		virtual ~GrabObjectRequest() {}

	protected:
		CharacterType character;
		int objectId;
		Vector3dInfo handPosition;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->objectId;
			archive & this->handPosition;
		}
	};

	typedef SharedPointer<GrabObjectRequest> GrabObjectRequestPtr;
}

#endif
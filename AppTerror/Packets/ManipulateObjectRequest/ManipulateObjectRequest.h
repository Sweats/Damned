#ifndef TERROR_MANIPULATE_OBJECT_REQUEST_H
#define TERROR_MANIPULATE_OBJECT_REQUEST_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class ManipulateObjectRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		ManipulateObjectRequest() {}
		ManipulateObjectRequest(CharacterType character, const Vector3d& handPosition)
			: character(character), handPosition(handPosition) {}

		virtual int getLabel() const {
			return MANIPULATE_OBJECT_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}
		
		Vector3d getHandPosition() {
			return this->handPosition.buildVector3d();
		}

		virtual ~ManipulateObjectRequest() {}

	protected:
		CharacterType character;
		Vector3dInfo handPosition;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->handPosition;
		}
	};

	typedef SharedPointer<ManipulateObjectRequest> ManipulateObjectRequestPtr;
}

#endif
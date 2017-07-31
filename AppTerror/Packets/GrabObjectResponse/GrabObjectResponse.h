#ifndef TERROR_GRAB_OBJECT_RESPONSE_H
#define TERROR_GRAB_OBJECT_RESPONSE_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	enum GrabObjectError {
		NO_GRAB_OBJECT_ERROR,
		GRAB_OBJECT_LOCKED
	};

	class GrabObjectResponse : public Sendable {
	public:
		friend class boost::serialization::access;

		GrabObjectResponse() {}
		GrabObjectResponse(GrabObjectError error, int objectId, CharacterType character)
			: error(error), objectId(objectId), character(character) {}

		virtual int getLabel() const {
			return GRAB_OBJECT_RESPONSE;
		}

		GrabObjectError getError() {
			return this->error;
		}

		int getObjectId() {
			return this->objectId;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		virtual ~GrabObjectResponse() {}

	protected:
		GrabObjectError error;
		int objectId;
		CharacterType character;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->error;
			archive & this->objectId;
			archive & this->character;
		}
	};

	typedef SharedPointer<GrabObjectResponse> GrabObjectResponsePtr;
}

#endif
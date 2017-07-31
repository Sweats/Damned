#ifndef TERROR_RELEASE_OBJECT_REQUEST_H 
#define TERROR_RELEASE_OBJECT_REQUEST_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	class ReleaseObjectRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		ReleaseObjectRequest() {}
		ReleaseObjectRequest(CharacterType character) : character(character) {}

		virtual int getLabel() const {
			return RELEASE_OBJECT_REQUEST;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		virtual ~ReleaseObjectRequest() {}

	protected:
		CharacterType character;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
		}
	};

	typedef SharedPointer<ReleaseObjectRequest> ReleaseObjectRequestPtr;
}

#endif
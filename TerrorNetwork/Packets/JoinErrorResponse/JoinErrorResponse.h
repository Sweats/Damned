#ifndef TERROR_NETWORK_JOIN_ERROR_RESPONSE_H
#define TERROR_NETWORK_JOIN_ERROR_RESPONSE_H

#include "Packets/PacketsEnum.h"

namespace terror_network {
	enum JoinError {
		JOIN_HOST_TIMEOUT,
		JOIN_INVALID_PASSWORD,
		JOIN_HOST_FULL
	};

	class JoinErrorResponse : public Sendable {
	public:
		friend class boost::serialization::access;

		JoinErrorResponse() {}

		JoinErrorResponse(JoinError joinError) : joinError(joinError) {}

		virtual int getLabel() const {
			return JOIN_ERROR_RESPONSE;
		}

		JoinError getJoinError() {
			return this->joinError;
		}

		virtual ~JoinErrorResponse() {}

	protected:
		JoinError joinError;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->joinError;
		}
	};

	typedef SharedPointer<JoinErrorResponse> JoinErrorResponsePtr;
}

#endif
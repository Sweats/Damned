#ifndef TERROR_NETWORK_JOIN_ACCEPTED_H
#define TERROR_NETWORK_JOIN_ACCEPTED_H

#include "Packets/PacketsEnum.h"

namespace terror_network{
	class JoinAccepted : public Sendable {
	public:
		friend class boost::serialization::access;

		JoinAccepted() {}

		virtual int getLabel() const {
			return JOIN_ACCEPTED;
		}

		virtual ~JoinAccepted() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {}
	};

	typedef SharedPointer<JoinAccepted> JoinAcceptedPtr;
}

#endif
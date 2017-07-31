#ifndef TERROR_RESET_SCARE_TRAP_REQUEST_H
#define TERROR_RESET_SCARE_TRAP_REQUEST_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class ResetScareTrapRequest : public Sendable {
	public:
		friend class boost::serialization::access;

		ResetScareTrapRequest() : relativeAmount(0.5f), autoReset(false), autoResetTime(0.0f) {}
		ResetScareTrapRequest(float relativeAmount, bool autoReset, float autoResetTime) : relativeAmount(relativeAmount), autoReset(autoReset), autoResetTime(autoResetTime) {}

		virtual int getLabel() const {
			return RESET_SCARE_TRAP_REQUEST;
		}

		float getRelativeAmount() {
			return this->relativeAmount;
		}

		bool getAutoReset() {
			return this->autoReset;
		}

		float getAutoResetTime() {
			return this->autoResetTime;
		}

		virtual ~ResetScareTrapRequest() {}

	protected:
		float relativeAmount;
		bool autoReset;
		float autoResetTime;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->relativeAmount;
			archive & this->autoReset;
			archive & this->autoResetTime;
		}
	};

	typedef SharedPointer<ResetScareTrapRequest> ResetScareTrapRequestPtr;
}

#endif
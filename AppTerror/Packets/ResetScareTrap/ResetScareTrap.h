#ifndef TERROR_RESET_SCARE_TRAP_H
#define TERROR_RESET_SCARE_TRAP_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class ResetScareTrap : public Sendable {
	public:
		friend class boost::serialization::access;

		ResetScareTrap() {}
		ResetScareTrap(int seed, float relativeAmount, bool autoReset, float autoResetTime) : seed(seed), relativeAmount(relativeAmount), autoReset(autoReset), autoResetTime(autoResetTime) {}

		virtual int getLabel() const {
			return RESET_SCARE_TRAP;
		}

		int getSeed() {
			return this->seed;
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

		virtual ~ResetScareTrap() {}

	protected:
		int seed;
		float relativeAmount;
		bool autoReset;
		float autoResetTime;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->seed;
			archive & this->relativeAmount;
			archive & this->autoReset;
			archive & this->autoResetTime;
		}
	};

	typedef SharedPointer<ResetScareTrap> ResetScareTrapPtr;
}

#endif
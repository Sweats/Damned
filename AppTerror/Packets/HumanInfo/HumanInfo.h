#ifndef TERROR_HUMAN_INFO_H
#define TERROR_HUMAN_INFO_H

#include "Packets/CharacterStateInfo/CharacterStateInfo.h"
#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {	
	struct HumanInfo : public Sendable{
		friend class boost::serialization::access;

		typedef SharedPointer<HumanInfo> HumanInfoPtr;

		HumanInfo() : stateInfo(new CharacterStateInfo), flashlightState(false), flashlightBatteryCharge(1.0f), flashlightIntensityFactor(1.0f) {}

		HumanInfo(CharacterType character) : stateInfo(new CharacterStateInfo), character(character), flashlightState(false), flashlightBatteryCharge(1.0f), flashlightIntensityFactor(1.0f) {}

		virtual int getLabel() const {
			return HUMAN_INFO;
		}

		void setEyeDirection(const Vector3d& eyeDirection) {
			this->eyeDirection.setVector3d(eyeDirection);
		}

		Vector3d getEyeDirection() const {
			return this->eyeDirection.buildVector3d();
		}

		CharacterType character;
		CharacterStateInfoPtr stateInfo;		
		Vector3dInfo eyeDirection;
		bool flashlightState;
		float flashlightBatteryCharge;
		float flashlightIntensityFactor;

		virtual ~HumanInfo() {}

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->character;
			archive & this->stateInfo;			
			archive & this->eyeDirection;
			archive & this->flashlightState;
			archive & this->flashlightBatteryCharge;
			archive & this->flashlightIntensityFactor;
		}
	};

	typedef HumanInfo::HumanInfoPtr HumanInfoPtr;
}

#endif
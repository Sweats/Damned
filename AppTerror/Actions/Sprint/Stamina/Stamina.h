#ifndef TERROR_STAMINA_H
#define TERROR_STAMINA_H

#include "Config.h"

namespace terror {
	class Stamina {
	public:
		Stamina(float max, float regenerationRate, float consumptionRate, float minNeeded)		
			: max(max), regenerationRate(regenerationRate), consumptionRate(consumptionRate), minNeeded(minNeeded), value(max) {}

		void consume(float diffTime) {
			this->value -= diffTime*this->consumptionRate;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		void regenerate(float diffTime) {
			this->value += diffTime*this->regenerationRate;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		bool isSufficient() {
			return this->value >= this->minNeeded;
		}

		bool isNull() {
			return fabs(this->value) <= 1.0e-4f;
		}

		void setValue(float value) {
			this->value = clamp(value, 0.0, this->max);
		}

		virtual ~Stamina() {}

	protected:
		float max;
		float regenerationRate;
		float consumptionRate;
		float minNeeded;
		float value;
	};

	typedef SharedPointer<Stamina> StaminaPtr;
}

#endif
#ifndef TERROR_ENERGY_H
#define TERROR_ENERGY_H

#include "Config.h"

namespace terror {
	class Energy {
	public:
		Energy(float max, float regenerationRate, float consumptionRate) 
			: max(max), regenerationRate(regenerationRate), consumptionRate(consumptionRate), value(0.0f) {}

		void setConsumptionRate(float consumptionRate) {
			this->consumptionRate = consumptionRate;
		}

		float getConsumptionRate() {
			return this->consumptionRate;
		}

		void setRegenerationRate(float regenerationRate) {
			this->regenerationRate = regenerationRate;
		}

		float getRegenerationRate() {
			return this->regenerationRate;
		}

		void consume(float diffTime) {
			this->value -= diffTime*this->consumptionRate;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		void regenerate(float diffTime) {
			this->value += diffTime*this->regenerationRate;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		void recover(float value) {
			this->value += value;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		void spend(float value) {
			this->value -= value;
			this->value = clamp(this->value, 0.0f, this->max);
		}

		void consumeAll() {
			this->value = 0.0f;
		}

		bool isNull() {
			return fabs(this->value) <= 1.0e-4f;
		}

		bool isMax() {
			return fabs(this->max - this->value) <= 1.0e-4f;
		}

		float getValue() {
			return this->value;
		}

		void setValue(float value) {
			if (value > this->max || value < 0.0f) {
				throw runtime_error("Trying to set a value out of range in class Energy.");
			}
			this->value = value;
		}

		virtual ~Energy() {}

	protected:
		float max;
		float regenerationRate; 
		float consumptionRate;
		float value;
	};

	typedef SharedPointer<Energy> EnergyPtr;
}

#endif
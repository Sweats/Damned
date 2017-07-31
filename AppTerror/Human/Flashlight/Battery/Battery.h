#ifndef TERROR_BATTERY_H
#define TERROR_BATTERY_H

#include "Config.h"

namespace terror {
	class Battery {
	public:
		Battery(float lifetime = 420.0f, float initialCharge = 1.0f)
	
			: charge(initialCharge), discharging(true), lifetime(lifetime)
		{
			this->restoreDefaultConsumptionRate();
		}

		float getCharge() {
			return this->charge;
		}

		void recharge() {
			this->charge = 1.0f;
		}

		void setCharge(float charge) {
			this->charge = charge;
			this->charge = clamp(charge, 0.0f, 1.0f);
		}

		void addCharge(float charge) {
			this->charge += charge;
			this->charge = clamp(this->charge, 0.0f, 1.0f);
		}

		void start() {
			this->discharging = true;
		}

		void stop() {
			this->discharging = false;
		}

		void increaseConsumptionRate(float additionalConsumptionRate) {
			if (additionalConsumptionRate > 0.0f) { 
				this->consumptionRate += additionalConsumptionRate;
			}
		}

		float getConsumptionRate() {
			return this->consumptionRate;
		}

		void restoreDefaultConsumptionRate() {
			this->consumptionRate = 1.0f/this->lifetime;
		}

		void update(float diffTime) {
			if (!this->discharging) {
				return;
			}

			this->charge -= diffTime*this->consumptionRate;
			this->charge = clamp(this->charge, 0.0f, 1.0f);
		}

	protected:
		float charge;
		bool discharging;
		float lifetime;
		float consumptionRate;
	};

	typedef SharedPointer<Battery> BatteryPtr;
}

#endif
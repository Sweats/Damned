#ifndef TERROR_INTERPOLATOR_H
#define TERROR_INTERPOLATOR_H

#include "Config.h"

namespace terror {
	class Interpolator {
	public:
		Interpolator(float x0, float y0, float x1, float y1)
			: x0(x0), y0(y0), x1(x1), y1(y1) 
		{
			this->updateMinMaxYValues();	
		}

		void setFirstPoint(float x0, float y0) {
			this->x0 = x0;
			this->y0 = y0;
			this->updateMinMaxYValues();
		}

		void setSecondPoint(float x1, float y1) {
			this->x1 = x1;
			this->y1 = y1;
			this->updateMinMaxYValues();
		}

		virtual float interpolate(const float& x) = 0;

		virtual ~Interpolator() {}

	protected:
		float x0, y0;
		float x1, y1;
		float minY, maxY;

	private:
		void updateMinMaxYValues(){
			this->minY = std::min(this->y0, this->y1);
			this->maxY = std::max(this->y0, this->y1);
		}

	};

	typedef SharedPointer<Interpolator> InterpolatorPtr;
}

#endif
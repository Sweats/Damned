#ifndef TERROR_EXPONENTIAL_INTERPOLATOR_H
#define TERROR_EXPONENTIAL_INTERPOLATOR_H

#include "MathModels/Interpolator/Interpolator.h"

namespace terror {
	class ExponentialInterpolator : public Interpolator {
	public:
		ExponentialInterpolator(float x0, float y0, float x1, float y1, float alpha = 1.0f)
			: Interpolator(x0, y0, x1, y1), alpha(alpha) {}

		float interpolate(const float& x) {
			float normalX = (x-x0)/(x1-x0);
			float factor = (1-exp(-alpha*normalX))/(1-exp(-alpha));
			return clamp((1-factor)*y0 + factor*y1, minY, maxY);
		}

		virtual ~ExponentialInterpolator() {}

	protected:
		float alpha;
	};

	typedef SharedPointer<ExponentialInterpolator> ExponentialInterpolatorPtr;
}

#endif
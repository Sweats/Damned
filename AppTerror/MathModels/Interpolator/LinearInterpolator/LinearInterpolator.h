#ifndef TERROR_LINEAR_INTERPOLATOR_H
#define TERROR_LINEAR_INTERPOLATOR_H

#include "MathModels/Interpolator/Interpolator.h"

namespace terror {
	class LinearInterpolator : public Interpolator {
	public:
		LinearInterpolator(float x0, float y0, float x1, float y1)
			: Interpolator(x0, y0, x1, y1) {}

		float interpolate(const float& x) {
			float factor = (x-x0)/(x1-x0);
			return clamp((1-factor)*y0 + factor*y1, minY, maxY);
		}

		virtual ~LinearInterpolator() {}
	};

	typedef SharedPointer<LinearInterpolator> LinearInterpolatorPtr;
}

#endif
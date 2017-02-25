#include "stdafx.h"
#include "UtilFunctions.h"

namespace RayTracer
{
	// Quake's fast inverse square root equation. Psuedo-code found here: http://en.wikipedia.org/wiki/Fast_inverse_square_root
	float UtilFunctions::InvSqrt(float x) {
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = x * 0.5F;
		y = x;
		i = *(long *)&y;
		i = 0x5f3759df - (i >> 1);
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y));

		return y;
	}
}
#pragma once
#include "Vector.h"
#include "Windows.h"
#include "WorldObject.h"
#include "Plane.h"

namespace RayTracer
{
	class RectangularPlane : public Plane
	{
		public:
			// These constructors take four points that lie on the plane
			RectangularPlane(Vector p1, Vector p2, Vector p3, Vector p4, COLORREF color, float alpha);
			RectangularPlane(Vector p1, Vector p2, Vector p3, Vector p4, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight);

		private:

			float m_minX;
			float m_maxX;

			float m_minY;
			float m_maxY;

			static bool SortByX(const Vector &v1, const Vector &v2);
			static bool SortByY(const Vector &v1, const Vector &v2);

			void SetMinAndMaxValues(Vector p1, Vector p2, Vector p3, Vector p4);
	};
}
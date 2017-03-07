#pragma once
#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	class RectangularPlane
	{
		public:
			// These constructors take four points that lie on the plane
			RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, COLORREF color, float alpha);
			RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight);
			
			bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			COLORREF GetColorAt(const Vector &point) const;
			Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const;
			bool equals(const RectangularPlane &other) const;
			float GetAlpha() const;

		private:
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			// Transparency of object. Ranges from 1 = completely opaque to 0 = completely transparent.
			float m_alpha;

			float m_a;
			float m_b;
			float m_c;
			float m_d;

			float m_repeatWidth;
			float m_repeatHeight;

			void GetCoefficients(const Vector &p1, const Vector &p2, const Vector &p3, float(&coefficients)[4]); // Fills coefficients array with four values representing the coefficients a, b, c, and d for the plane created from the three points p1, p2, and p3
			void SetCoefficients(float a, float b, float c, float d);

			float m_minX;
			float m_maxX;

			float m_minY;
			float m_maxY;

			static bool SortByX(const Vector &v1, const Vector &v2);
			static bool SortByY(const Vector &v1, const Vector &v2);

			void SetMinAndMaxValues(Vector p1, Vector p2, Vector p3, Vector p4);
	};
}
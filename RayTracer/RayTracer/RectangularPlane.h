#pragma once
#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	class RectangularPlane
	{
		public:
			// These constructors take four points that lie on the plane
			RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, COLORREF color, float alpha, float refraction, float reflectivity);
			RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const cimg_library::CImg<unsigned char> &texture, float alpha, float refraction, float reflectivity, float repeatWidth, float repeatHeight);
			
			bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			COLORREF GetColorAt(const Vector &point) const;

			// Returns the normalized normal vector at the given point
			Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const;

			// Returns the center point of the plane. Used when calculating light reflections off this plane.
			const Vector& GetCenter() const;

			bool equals(const RectangularPlane &other) const;
			float GetAlpha() const;
			float GetRefraction() const;
			float GetReflectivity() const;
			float GetA() const;
			float GetB() const;
			float GetC() const;
			float GetD() const;

		private:
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			// Transparency of object. Ranges from 1 = completely opaque to 0 = completely transparent.
			float m_alpha;

			// Reflectivity of object. Ranges from 1.0 = completely reflective to 0 = completely unreflective.
			float m_reflectivity;

			// Refracive index of object. Ranges from 2.0 = max refraction to 1.0 = no refraction.
			float m_refraction;

			float m_a;
			float m_b;
			float m_c;
			float m_d;

			Vector m_center;

			float m_repeatWidth;
			float m_repeatHeight;

			void GetCoefficients(const Vector &p1, const Vector &p2, const Vector &p3, float(&coefficients)[4]); // Fills coefficients array with four values representing the coefficients a, b, c, and d for the plane created from the three points p1, p2, and p3
			void SetCoefficients(float a, float b, float c, float d);

			void SetMinAndMaxValues(Vector p1, Vector p2, Vector p3, Vector p4);
			void SetCenter(Vector p1, Vector p2, Vector p3, Vector p4);

			float m_minX;
			float m_maxX;

			float m_minY;
			float m_maxY;

			static bool SortByX(const Vector &v1, const Vector &v2);
			static bool SortByY(const Vector &v1, const Vector &v2);
	};
}
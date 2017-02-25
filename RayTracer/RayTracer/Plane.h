#pragma once
#include "Vector.h"
#include "Windows.h"
#include "WorldObject.h"

namespace RayTracer
{
	class Plane : public WorldObject
	{
		public:
			// These constructors take coefficients a, b, c, and d to fit the equation aX + bY + cZ + d = 0
			Plane(float a, float b, float c, float d, COLORREF color, float alpha);
			Plane(float a, float b, float c, float d, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight);
	
			// These constructors take three points that lie on the plane
			Plane(Vector p1, Vector p2, Vector p3, COLORREF color, float alpha);
			Plane(Vector p1, Vector p2, Vector p3, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight);
	
			virtual bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const override;
			virtual COLORREF GetColorAt(const Vector &point) const override;
			virtual Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const override;
			virtual bool equals(const WorldObject &other) const override;
			virtual float GetAlpha() const override;

		private:
			float m_a;
			float m_b;
			float m_c;
			float m_d;

			float m_repeatWidth;
			float m_repeatHeight;

			void GetCoefficients(const Vector &p1, const Vector &p2, const Vector &p3, float (&coefficients)[4]); // Fills coefficients array with four values representing the coefficients a, b, c, and d for the plane created from the three points p1, p2, and p3
			void SetCoefficients(float a, float b, float c, float d);
	};
}
#pragma once
#include "stdafx.h"

#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	class Sphere
	{
		public:
			Sphere(Vector center, float radius, COLORREF color, float alpha, float refraction, float reflectivity);
			Sphere(Vector center, float radius, const cimg_library::CImg<unsigned char> &texture, float alpha, float refraction, float reflectivity);

			bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			COLORREF GetColorAt(const Vector &point) const;

			// Returns the normalized normal vector at the given point
			Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const;

			bool equals(const Sphere &other) const;
			float GetAlpha() const;
			float GetRefraction() const;
			float GetReflectivity() const;

		private:
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			// Transparency of object. Ranges from 1 = completely opaque to 0 = completely transparent.
			float m_alpha;

			// Reflectivity of object. Ranges from 1.0 = completely reflective to 0 = completely unreflective.
			float m_reflectivity;

			// Refracive index of object. Ranges from 2.0 = max refraction to 1.0 = no refraction.
			float m_refraction;

			Vector m_center;
			float m_radius;

			static const float m_Pi; // Pi
			static const float m_Two_Pi; // 2 * Pi
	};
}
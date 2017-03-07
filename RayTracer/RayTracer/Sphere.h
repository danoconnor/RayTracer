#pragma once
#include "stdafx.h"

#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	class Sphere
	{
		public:
			Sphere(Vector center, float radius, COLORREF color, float alpha);
			Sphere(Vector center, float radius, const cimg_library::CImg<unsigned char> &texture, float alpha);

			bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			COLORREF GetColorAt(const Vector &point) const;
			Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const;
			bool equals(const Sphere &other) const;
			float GetAlpha() const;

		private:
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			// Transparency of object. Ranges from 1 = completely opaque to 0 = completely transparent.
			float m_alpha;

			Vector m_center;
			float m_radius;

			static const float m_Pi; // Pi
			static const float m_Two_Pi; // 2 * Pi
	};
}
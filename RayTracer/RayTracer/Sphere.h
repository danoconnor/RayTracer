#pragma once
#include "stdafx.h"

#include "Vector.h"
#include "Windows.h"
#include "WorldObject.h"

namespace RayTracer
{
	class Sphere : public WorldObject
	{
		public:
			Sphere(Vector center, float radius, COLORREF color, float alpha);
			Sphere(Vector center, float radius, const cimg_library::CImg<unsigned char> &texture, float alpha);

			virtual bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const override;
			virtual COLORREF GetColorAt(const Vector &point) const override;
			virtual Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const override;
			virtual bool equals(const WorldObject &other) const override;
			virtual float GetAlpha() const override;

		private:
			Vector m_center;
			float m_radius;

			static const float m_Pi; // Pi
			static const float m_Two_Pi; // 2 * Pi
	};
}
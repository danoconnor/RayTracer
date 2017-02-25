#pragma once

#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	class WorldObject {
		public:
			WorldObject(COLORREF color, const cimg_library::CImg<unsigned char> *texture, float alpha);

			virtual bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			virtual COLORREF GetColorAt(const Vector &point) const;
			virtual Vector GetNormalAt(const Vector &point, const Vector &lookDir) const;
			virtual bool equals(const WorldObject &other) const;
			virtual float GetAlpha() const;

		protected:
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			float m_alpha; // Transparency of object. Ranges from 1 = completely opaque to 0 = completely transparent.
	};
}
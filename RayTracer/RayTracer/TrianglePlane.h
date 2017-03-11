#pragma once

#include "CImg-2.0/CImg.h"

#include "Vector.h"
#include "Windows.h"
#include <vector>

namespace RayTracer
{
	class TrianglePlane
	{
		public:
			TrianglePlane(const Vector &v1, const Vector &v2, const Vector &v3, COLORREF color, float alpha);
			TrianglePlane(const Vector &v1, const Vector &v2, const Vector &v3, const cimg_library::CImg<unsigned char> &texture, float alpha, float uPos1, float vPos1, float uPos2, float vPos2, float uPos3, float vPos3);

			bool CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const;
			COLORREF GetColorAt(const Vector &point) const;
			Vector GetNormalAt(const Vector &point, const  Vector &lookDir) const;
			bool equals(const TrianglePlane &other) const;
			float GetAlpha() const;
			std::vector<Vector> GetPoints();

		private:
			Vector m_v0;
			Vector m_v1;
			Vector m_v2;

			// Represent the uv coordinates of the triangle in the texture
			float m_uPos0;
			float m_vPos0;

			float m_uPos1;
			float m_vPos1;

			float m_uPos2;
			float m_vPos2;

			// Used for ray-triangle intersections - is this worth the additional 24 bytes in memory?
			Vector m_edge1; // m_v1 - m_v0
			Vector m_edge2; // m_v2 - m_v0

			// TODO - Union these to save 8 bytes - will then need some way to determine if object has a texture or not (alpha isn't used in COLORREF, so there's an open byte there and alignment of the pointer could free a bit for a boolean)
			const cimg_library::CImg<unsigned char> *m_texture;
			COLORREF m_color;

			// Transparency of object. Ranges from 255 = completely opaque to 0 = completely transparent.
			float m_alpha;
	};
}
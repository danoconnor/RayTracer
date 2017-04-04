#include "stdafx.h"

#include "TrianglePlane.h"

#define EPSILON 0.000001

namespace RayTracer
{
	TrianglePlane::TrianglePlane(const Vector &v1, const Vector &v2, const Vector &v3, COLORREF color, float alpha, float refraction, float reflectivity)
	{
		m_v0 = v1;
		m_v1 = v2;
		m_v2 = v3;

		m_edge1 = m_v1 - m_v0;
		m_edge2 = m_v2 - m_v0;

		m_color = color;
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;
		m_texture = nullptr;
	}

	TrianglePlane::TrianglePlane(const Vector &v1, const Vector &v2, const Vector &v3, const cimg_library::CImg<unsigned char> &texture, float alpha, float refraction, float reflectivity, float uPos1, float vPos1, float uPos2, float vPos2, float uPos3, float vPos3)
	{
		m_v0 = v1;
		m_v1 = v2;
		m_v2 = v3;

		m_uPos0 = uPos1;
		m_vPos0 = vPos1;

		m_uPos1 = uPos2;
		m_vPos1 = vPos2;

		m_uPos2 = uPos3;
		m_vPos2 = vPos3;

		m_edge1 = m_v1 - m_v0;
		m_edge2 = m_v2 - m_v0;

		m_color = RGB(0, 0, 0);
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;
		m_texture = &texture;
	}

	// Based on the Moller-Trumbore "Fast, Minimum Storage Ray/Triangle Intersection"
	bool TrianglePlane::CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const
	{
		Vector pVec = Vector::Cross(direction, m_edge2);

		float det = Vector::Dot(m_edge1, pVec);
		if (det > -EPSILON && det < EPSILON)
		{
			return false;
		}

		float invDet = 1.f / det;

		Vector tVec = origin - m_v0;

		float u = Vector::Dot(tVec, pVec) * invDet;
		if (u < 0.f || u > 1.f)
		{
			return false;
		}

		Vector qVec = Vector::Cross(tVec, m_edge1);

		float v = Vector::Dot(direction, qVec) * invDet;
		if (v < 0.f || u + v > 1.f)
		{
			return false;
		}

		distance = Vector::Dot(m_edge2, qVec) * invDet;

		if (distance < 0)
		{
			return false;
		}

		Vector scaledDirection = direction;
		scaledDirection.scalarMult(distance);

		collisionPoint = origin + scaledDirection;
		return true;
	}

	COLORREF TrianglePlane::GetColorAt(const Vector &point) const
	{
		if (m_texture)
		{
			// Map point to barycentric coordinates
			float u = ((m_v1.m_y - m_v2.m_y) * (point.m_x - m_v2.m_x) + (m_v2.m_x - m_v1.m_x) * (point.m_y - m_v2.m_y)) / ((m_v1.m_y - m_v2.m_y) * (m_v0.m_x - m_v2.m_x) + (m_v2.m_x - m_v1.m_x) * (m_v0.m_y - m_v2.m_y));
			float v = ((m_v2.m_y - m_v0.m_y) * (point.m_x - m_v2.m_x) + (m_v0.m_x - m_v2.m_x) * (point.m_y - m_v2.m_y)) / ((m_v1.m_y - m_v2.m_y) * (m_v0.m_x - m_v2.m_x) + (m_v2.m_x - m_v1.m_x) * (m_v0.m_y - m_v2.m_y));
			float w = 1 - u - v;

			float tCX = u * m_uPos0 + v * m_uPos1 + w * m_uPos2;
			float tCY = u * m_vPos0 + v * m_vPos1 + w * m_vPos2;

			int x = (int)floor(tCX * m_texture->width());
			int y = (int)floor(tCY * m_texture->height());

			const UINT8 red = *(m_texture->data(x, y, 0, 0));
			const UINT8 green = *(m_texture->data(x, y, 0, 1));
			const UINT8 blue = *(m_texture->data(x, y, 0, 2));

			return RGB(red, green, blue);
		}
		else
		{
			return m_color;
		}
	}

	Vector TrianglePlane::GetNormalAt(const Vector &point, const  Vector &lookDir) const
	{
		Vector normal = Vector::Cross(m_v1 - m_v0, m_v2 - m_v0);
		normal.normalize();

		float dot = lookDir.dot(normal);
		if (dot > 0)
		{
			normal.m_x *= -1;
			normal.m_y *= -1;
			normal.m_z *= -1;
		}

		return normal;
	}

	bool TrianglePlane::equals(const TrianglePlane &other) const
	{
		if ((void *)this == (void *)&other)
		{
			return true;
		}

		const TrianglePlane *otherTriangle = static_cast<const TrianglePlane *>(&other);

		if (m_v0 != otherTriangle->m_v0)
		{
			return false;
		}

		if (m_v1 != otherTriangle->m_v1)
		{
			return false;
		}

		if (m_v2 != otherTriangle->m_v2)
		{
			return false;
		}

		return true;
	}

	float TrianglePlane::GetAlpha() const
	{
		return m_alpha;
	}

	float TrianglePlane::GetRefraction() const
	{
		return m_refraction;
	}

	float TrianglePlane::GetReflectivity() const
	{
		return m_reflectivity;
	}

	std::vector<Vector> TrianglePlane::GetPoints()
	{
		std::vector<Vector> points;
		points.push_back(m_v0);
		points.push_back(m_v1);
		points.push_back(m_v2);

		return points;
	}
}
#include "stdafx.h"
#include "Plane.h"

namespace RayTracer
{
	Plane::Plane(float a, float b, float c, float d, COLORREF color, float alpha) : WorldObject(color, nullptr, alpha)
	{
		SetCoefficients(a, b, c, d);
	}

	Plane::Plane(float a, float b, float c, float d, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight) : WorldObject(RGB(0, 0, 0), &texture, alpha)
	{
		SetCoefficients(a, b, c, d);

		m_repeatHeight = repeatHeight;
		m_repeatWidth = repeatWidth;
	}

	Plane::Plane(Vector p1, Vector p2, Vector p3, COLORREF color, float alpha) : WorldObject(color, nullptr, alpha)
	{
		float coefficients[4];
		GetCoefficients(p1, p2, p3, coefficients);

		SetCoefficients(coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
	}

	Plane::Plane(Vector p1, Vector p2, Vector p3, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight) : WorldObject(RGB(0, 0, 0), &texture, alpha)
	{
		float coefficients[4];
		GetCoefficients(p1, p2, p3, coefficients);

		m_repeatHeight = repeatHeight;
		m_repeatWidth = repeatWidth;

		SetCoefficients(coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
	}

	bool Plane::CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const
	{
		float t = (-1.f*m_d - m_a*origin.m_x - m_b*origin.m_y - m_c*origin.m_z) / (m_a*direction.m_x + m_b*direction.m_y + m_c*direction.m_z);

		if (t < 0)
		{
			return false;
		}

		distance = t;

		collisionPoint.m_x = origin.m_x + distance*direction.m_x;
		collisionPoint.m_y = origin.m_y + distance*direction.m_y;
		collisionPoint.m_z = origin.m_z + distance*direction.m_z;

		return true;
	}

	COLORREF Plane::GetColorAt(const Vector &point) const
	{
		if (m_texture)
		{
			float ratioX = point.m_x / m_repeatWidth;
			int x = (int)floor((ratioX - floor(ratioX)) * m_texture->width());

			float ratioY = point.m_y / m_repeatHeight;
			int y = m_texture->height() - (int)floor((ratioY - floor(ratioY)) * m_texture->height()) - 1;

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

	Vector Plane::GetNormalAt(const Vector &point, const  Vector &lookDir) const
	{
		Vector normal(m_a, m_b, m_c);
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

	bool Plane::equals(const WorldObject &other) const
	{
		if ((void *)this == (void *)&other)
		{
			return true;
		}

		const Plane *otherPlane = static_cast<const Plane *>(&other);

		if (otherPlane->m_a != m_a)
		{
			return false;
		}

		if (otherPlane->m_b != m_b)
		{
			return false;
		}

		if (otherPlane->m_c != m_c)
		{
			return false;
		}

		if (otherPlane->m_d != m_d)
		{
			return false;
		}

		return true;
	}

	float Plane::GetAlpha() const
	{
		return m_alpha;
	}

	void Plane::GetCoefficients(const Vector &p1, const Vector &p2, const Vector &p3, float(&coefficients)[4])
	{
		Vector v1 = p1 - p2;
		Vector v2 = p1 - p3;

		Vector normal = Vector::Cross(v1, v2);

		float d = -1 * Vector::Dot(normal, p1);

		coefficients[0] = normal.m_x;
		coefficients[1] = normal.m_y;
		coefficients[2] = normal.m_z;
		coefficients[3] = d;
	}

	void Plane::SetCoefficients(float a, float b, float c, float d)
	{
		m_a = a;
		m_b = b;
		m_c = c;
		m_d = d;
	}
}
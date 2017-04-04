#include "stdafx.h"
#include "RectangularPlane.h"

namespace RayTracer
{
	RectangularPlane::RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, COLORREF color, float alpha, float refraction, float reflectivity)
	{
		m_color = color;
		m_texture = nullptr;
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;

		float coefficients[4];
		GetCoefficients(p1, p2, p3, coefficients);

		SetCoefficients(coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
		SetMinAndMaxValues(p1, p2, p3, p4);
	}

	RectangularPlane::RectangularPlane(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const cimg_library::CImg<unsigned char> &texture, float alpha, float refraction, float reflectivity, float repeatWidth, float repeatHeight)
	{
		m_color = RGB(0, 0, 0);
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;
		m_texture = &texture;
		m_repeatWidth = repeatWidth;
		m_repeatHeight = repeatHeight;

		float coefficients[4];
		GetCoefficients(p1, p2, p3, coefficients);

		SetCoefficients(coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
		SetMinAndMaxValues(p1, p2, p3, p4);
	}

	bool RectangularPlane::CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const
	{
		float tDenominator = (m_a*direction.m_x + m_b*direction.m_y + m_c*direction.m_z);
		if (tDenominator == 0)
		{
			return false;
		}

		float t = (-1.f*m_d - m_a*origin.m_x - m_b*origin.m_y - m_c*origin.m_z) / tDenominator;

		if (t < 0)
		{
			return false;
		}

		distance = t;

		collisionPoint.m_x = origin.m_x + distance*direction.m_x;
		collisionPoint.m_y = origin.m_y + distance*direction.m_y;
		collisionPoint.m_z = origin.m_z + distance*direction.m_z;

		// See if the collision point falls within the rectangle bounds
		return (collisionPoint.m_x >= m_minX && collisionPoint.m_x <= m_maxX) && (collisionPoint.m_y >= m_minY && collisionPoint.m_y <= m_maxY);
	}

	COLORREF RectangularPlane::GetColorAt(const Vector &point) const
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

	Vector RectangularPlane::GetNormalAt(const Vector &point, const  Vector &lookDir) const
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

	bool RectangularPlane::equals(const RectangularPlane &other) const
	{
		if ((void *)this == (void *)&other)
		{
			return true;
		}

		const RectangularPlane *otherPlane = static_cast<const RectangularPlane *>(&other);

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

	float RectangularPlane::GetAlpha() const
	{
		return m_alpha;
	}

	float RectangularPlane::GetRefraction() const
	{
		return m_refraction;
	}

	float RectangularPlane::GetReflectivity() const
	{
		return m_reflectivity;
	}

	bool RectangularPlane::SortByX(const Vector &v1, const Vector &v2)
	{
		return v1.m_x < v2.m_x;
	}

	bool RectangularPlane::SortByY(const Vector &v1, const Vector &v2)
	{
		return v1.m_y < v2.m_y;
	}

	void RectangularPlane::GetCoefficients(const Vector &p1, const Vector &p2, const Vector &p3, float(&coefficients)[4])
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

	void RectangularPlane::SetCoefficients(float a, float b, float c, float d)
	{
		m_a = a;
		m_b = b;
		m_c = c;
		m_d = d;
	}

	void RectangularPlane::SetMinAndMaxValues(Vector p1, Vector p2, Vector p3, Vector p4)
	{
		Vector points[] = { p1, p2, p3, p4 };
		std::vector<Vector> pointList(points, points + 4);

		// Sort the point list by the x-values to easily find the min and max x-values
		std::sort(pointList.begin(), pointList.end(), &RectangularPlane::SortByX);

		m_minX = pointList[0].m_x;
		m_maxX = pointList[3].m_x;


		// Sort the point list by the y-values to easily find the min and max y-values
		std::sort(pointList.begin(), pointList.end(), &RectangularPlane::SortByY);

		m_minY = pointList[0].m_y;
		m_maxY = pointList[3].m_y;
	}
}
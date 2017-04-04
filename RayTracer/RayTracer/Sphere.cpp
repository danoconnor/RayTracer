#include "stdafx.h"
#include "Sphere.h"

#include "UtilFunctions.h"

namespace RayTracer
{
	const float Sphere::m_Pi = 3.14159265358979323846f;
	const float Sphere::m_Two_Pi = 6.28318530717958647692f;

	Sphere::Sphere(Vector center, float radius, COLORREF color, float alpha, float refraction, float reflectivity)
	{
		m_color = color;
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;
		m_texture = nullptr;

		m_center = center;
		m_radius = radius;
	}

	Sphere::Sphere(Vector center, float radius, const cimg_library::CImg<unsigned char> &texture, float alpha, float refraction, float reflectivity)
	{
		m_color = RGB(0, 0, 0);
		m_alpha = alpha;
		m_reflectivity = reflectivity;
		m_refraction = refraction;
		m_texture = &texture;

		m_center = center;
		m_radius = radius;
	}

	// Sets distance to the t-value where the vector collides with the sphere
	bool Sphere::CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const
	{
		Vector oMinusC = origin - m_center;

		float a = direction.dot(direction);
		float b = 2 * direction.dot(oMinusC);
		float c = oMinusC.dot(oMinusC) - m_radius*m_radius;

		float underSqrt = b*b - 4 * a*c;

		if (underSqrt <= 0)
		{
			return false;
		}

		float tLarge = (-1 * b + sqrt(underSqrt)) / (2 * a);

		if (tLarge <= 0)
		{
			return false;
		}

		float tSmall = (-1 * b - sqrt(underSqrt)) / (2 * a);
		if (tSmall <= 0)
		{
			distance = tLarge;
		}
		else
		{
			distance = tSmall;
		}

		collisionPoint.m_x = origin.m_x + distance*direction.m_x;
		collisionPoint.m_y = origin.m_y + distance*direction.m_y;
		collisionPoint.m_z = origin.m_z + distance*direction.m_z;

		return true;
	}

	COLORREF Sphere::GetColorAt(const Vector &point) const
	{
		if (m_texture)
		{
			Vector iMinusO = point - m_center;

			float u = 1 - (float)(0.5f + (atan2(iMinusO.m_z, iMinusO.m_x) / (m_Two_Pi)));
			float v = (float)(0.5f - (asin(iMinusO.m_y) / m_Pi));

			int x = (int)round(u * (m_texture->width() - 1));
			int y = (int)round(v * (m_texture->height() - 1));

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

	Vector Sphere::GetNormalAt(const Vector &point, const  Vector &lookDir) const
	{
		Vector normal = point - m_center;
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

	bool Sphere::equals(const Sphere &other) const
	{
		if ((void *)this == (void *)&other)
		{
			return true;
		}
		
		if (other.m_center != m_center)
		{
			return false;
		}
		else if (other.m_radius != m_radius)
		{
			return false;
		}
		else if (other.m_color != m_color)
		{
			return false;
		}

		return true;
	}

	float Sphere::GetAlpha() const
	{
		return m_alpha;
	}

	float Sphere::GetRefraction() const
	{
		return m_refraction;
	}

	float Sphere::GetReflectivity() const
	{
		return m_reflectivity;
	}
}
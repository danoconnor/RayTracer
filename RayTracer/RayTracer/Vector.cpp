#include "stdafx.h"
#include "Vector.h"

#include "UtilFunctions.h"

namespace RayTracer
{
	Vector::Vector()
	{
		m_x = 0;
		m_y = 0;
		m_z = 0;
	}

	Vector::Vector(float x, float y, float z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	float Vector::dot(const Vector &other) const
	{
		return m_x*other.m_x + m_y*other.m_y + m_z*other.m_z;
	}

	void Vector::scalarMult(float scalar)
	{
		m_x *= scalar;
		m_y *= scalar;
		m_z *= scalar;
	}

	void Vector::normalize()
	{
		float invMag = UtilFunctions::InvSqrt(m_x*m_x + m_y*m_y + m_z*m_z);

		m_x *= invMag;
		m_y *= invMag;
		m_z *= invMag;
	}

	Vector Vector::operator+(const Vector &other) const
	{
		return Vector(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
	}

	Vector Vector::operator-(const Vector &other) const
	{
		return Vector(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
	}

	bool Vector::operator==(const Vector &other) const
	{
		return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z;
	}

	bool Vector::operator!=(const Vector &other) const
	{
		return !(*this == other);
	}
}
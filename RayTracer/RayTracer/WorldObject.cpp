#include "stdafx.h"
#include "WorldObject.h"

namespace RayTracer
{
	WorldObject::WorldObject(COLORREF color, const cimg_library::CImg<unsigned char>* texture, float alpha)
	{
		m_color = color;
		m_texture = texture;
		m_alpha = alpha;
	}

	bool WorldObject::CheckCollision(const Vector &origin, const Vector &direction, float &distance, Vector &collisionPoint) const
	{
		return false;
	}

	COLORREF WorldObject::GetColorAt(const Vector &point) const
	{
		return RGB(0, 0, 0);
	}

	Vector WorldObject::GetNormalAt(const Vector &point, const  Vector &lookDir) const
	{
		return Vector();
	}

	bool WorldObject::equals(const WorldObject &other) const
	{
		return false;
	}

	float WorldObject::GetAlpha() const
	{
		return 1.f;
	}
}
#include "stdafx.h"
#include "RectangularPlane.h"

namespace RayTracer
{
	RectangularPlane::RectangularPlane(Vector p1, Vector p2, Vector p3, Vector p4, COLORREF color, float alpha) : Plane(p1, p2, p3, color, alpha)
	{
		SetMinAndMaxValues(p1, p2, p3, p4);
	}

	RectangularPlane::RectangularPlane(Vector p1, Vector p2, Vector p3, Vector p4, const cimg_library::CImg<unsigned char> &texture, float alpha, float repeatWidth, float repeatHeight) : Plane(p1, p2, p3, texture, alpha, repeatWidth, repeatHeight)
	{
		SetMinAndMaxValues(p1, p2, p3, p4);
	}

	bool RectangularPlane::SortByX(const Vector &v1, const Vector &v2)
	{
		return v1.m_x < v2.m_x;
	}

	bool RectangularPlane::SortByY(const Vector &v1, const Vector &v2)
	{
		return v1.m_y < v2.m_y;
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
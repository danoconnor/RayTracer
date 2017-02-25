#include "stdafx.h"
#include "LightSource.h"

namespace RayTracer
{
	LightSource::LightSource(Vector posOrDir)
	{
		m_posOrDir = posOrDir;
		m_posOrDir.normalize();

		m_color = RGB(0xff, 0xff, 0xff);
	}

	LightSource::LightSource(Vector posOrDir, COLORREF color)
	{
		m_posOrDir = posOrDir;
		m_posOrDir.normalize();

		m_color = color;
	}
}
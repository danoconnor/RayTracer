#include "stdafx.h"
#include "LightSource.h"

namespace RayTracer
{
	LightSource::LightSource(Vector posOrDir)
	{
		// TODO: posOrDir used to be normalized, but that broke pointlights (moved their position). Should this be normalized for suns?
		m_posOrDir = posOrDir;
		m_color = RGB(0xff, 0xff, 0xff);
	}

	LightSource::LightSource(Vector posOrDir, COLORREF color)
	{
		m_posOrDir = posOrDir;
		m_color = color;
	}
}
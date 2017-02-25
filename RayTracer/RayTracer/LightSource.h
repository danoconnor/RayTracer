#include "Vector.h"
#include "Windows.h"

namespace RayTracer
{
	// Note: For suns, the light direction should be the direction from the scene to the sun, not the direction from the sun to the scene
	class LightSource
	{
		public:
			LightSource(Vector posOrDir);
			LightSource(Vector posOrDir, COLORREF color);

			Vector &GetPosorDir() { return m_posOrDir; }
			COLORREF GetColor() { return m_color; }

		private:
			Vector m_posOrDir;
			COLORREF m_color;
	};
}
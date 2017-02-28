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

			const Vector &GetPosorDir() const { return m_posOrDir; }
			const COLORREF GetColor() const { return m_color; }

		private:
			Vector m_posOrDir;
			COLORREF m_color;
	};
}
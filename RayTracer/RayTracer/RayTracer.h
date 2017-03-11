#pragma once
#include "stdafx.h"

#include "World.h"

namespace RayTracer
{
	class RayTracer
	{
		public:
			RayTracer();
			~RayTracer();

			void Run();
			void Stop();

			void AddTriangle(const TrianglePlane *triangle);
			void AddSphere(const Sphere *sphere);
			void AddRectangle(const RectangularPlane* rectangle);
			void AddSun(const LightSource *sun);
			void AddPointLight(const LightSource *pointLight);

			// Loads a 3D object file from the filepath parameter.
			// Uses the Assimp library to parse the file. See here for supported file types: http://assimp.sourceforge.net/main_features_formats.html.
			void Add3DObject(const std::string &filePath);

			// Angles are in degrees
			void Add3DObject(const std::string &filePath, const Vector &translation, float rotationX, float rotationY, float rotationZ);

		private:
			void RunLoop();

			void DrawWorld();
			void ProcessUserInput();

			std::atomic<bool> m_isRunning;
			std::atomic<bool> m_outputFPS;

			SDL_Surface *m_surface;
			SDL_Window *m_window;

			World *m_world; 

			static const short Window_Height = 480;
			static const short Window_Width = 640;

			static const float Movement_Step;
			static const float Look_Step;
	};
}
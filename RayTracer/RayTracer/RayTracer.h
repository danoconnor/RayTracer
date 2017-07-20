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

			void SaveSceneToFile(const std::string &outputFilePath);

			void AddTriangle(const TrianglePlane *triangle);
			void AddSphere(const Sphere *sphere);
			void AddRectangle(const RectangularPlane* rectangle);
			void AddSun(const LightSource *sun);
			void AddPointLight(const LightSource *pointLight);

			// Loads a 3D object file from the filepath parameter.
			// Uses the Assimp library to parse the file. See here for supported file types: http://assimp.sourceforge.net/main_features_formats.html.
			void Add3DObject(const std::string &filePath);

			// Angles are in degrees
			void Add3DObject(const std::string &filePath, const Vector &translation, float rotationX, float rotationY, float rotationZ, float scale);

		private:
			void RunLoop();
			void CommandLineInputLoop();

			void DrawWorld();
			void ProcessWindowInput();
			void ProcessCommandLineInput();

			std::atomic<bool> m_isRunning;
			std::atomic<bool> m_outputFPS;

			SDL_Surface *m_surface;
			SDL_Window *m_window;

			World *m_world;

			std::mutex m_userInputLock;
			std::vector<std::string> m_userCommands;

			const short Window_Height = 480;
			const short Window_Width = 640;

			const short Surface_Pitch = 2560;
			const short Surface_Pixel_Size = 4;

			const float Movement_Step = 0.2f;
			const float Look_Step = 0.2f;
	};
}
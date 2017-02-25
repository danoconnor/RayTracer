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

			void AddObject(const WorldObject &object);
			void AddSun(const LightSource &sun);
			void AddPointLight(const LightSource &pointLight);

		private:
			void RunLoop();

			void DrawWorld();
			void ProcessUserInput();

			std::atomic<bool> m_isRunning;

			SDL_Surface *m_surface;
			SDL_Window *m_window;

			World *m_world; 

			static const short Window_Height = 480;
			static const short Window_Width = 640;

			static const float Movement_Step;
			static const float Look_Step;
	};
}
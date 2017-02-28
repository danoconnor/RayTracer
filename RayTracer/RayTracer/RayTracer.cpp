#include "stdafx.h"
#include "RayTracer.h"

const float RayTracer::RayTracer::Movement_Step = 0.2f;
const float RayTracer::RayTracer::Look_Step = 0.2f;

RayTracer::RayTracer::RayTracer()
{
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("Ray Tracer", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		Window_Width, 
		Window_Height, 
		SDL_WINDOW_SHOWN);

	m_surface = SDL_GetWindowSurface(m_window);

	m_world = new World();

	m_isRunning = false;
	m_outputFPS = true;
}

RayTracer::RayTracer::~RayTracer()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	delete m_world;
}

void RayTracer::RayTracer::Run()
{
	m_isRunning = true;
	RunLoop();
}

void RayTracer::RayTracer::Stop()
{
	m_isRunning = false;
}

void RayTracer::RayTracer::AddObject(const WorldObject* object)
{
	m_world->AddWorldObject(object);
}

void RayTracer::RayTracer::AddSun(const LightSource* sun)
{
	m_world->AddSun(sun);
}

void RayTracer::RayTracer::AddPointLight(const LightSource* pointLight)
{
	m_world->AddPointLight(pointLight);
}

void RayTracer::RayTracer::RunLoop()
{
	float fps = 0;
	long begin = 0;
	long end = 0;

	while (m_isRunning)
	{
		begin = GetTickCount();

		ProcessUserInput();
		DrawWorld();

		end = GetTickCount();

		if (begin != end)
		{
			fps = (1.f / (end - begin)) * 1000;
		}
		else
		{
			fps = 1000;
		}
		
		if (m_outputFPS)
		{
			printf("FPS: %f\n", fps);
		}
	}
}

void RayTracer::RayTracer::ProcessUserInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			Stop();
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
				case SDLK_UP:
				{
					m_world->LookUp(Look_Step);
					break;
				}
				case SDLK_DOWN:
				{
					m_world->LookDown(Look_Step);
					break;
				}
				case SDLK_RIGHT:
				{
					m_world->LookRight(Look_Step);
					break;
				}
				case SDLK_LEFT:
				{
					m_world->LookLeft(Look_Step);
					break;
				}
				case SDLK_q:
				{
					m_world->MoveDown(Movement_Step);
					break;
				}
				case SDLK_e:
				{
					m_world->MoveUp(Movement_Step);
					break;
				}
				case SDLK_w:
				{
					m_world->StepForward(Movement_Step);
					break;
				}
				case SDLK_s:
				{
					m_world->StepBackward(Movement_Step);
					break;
				}
				case SDLK_d:
				{
					m_world->StepRight(Movement_Step);
					break;
				}
				case SDLK_a:
				{
					m_world->StepLeft(Movement_Step);
					break;
				}
				case SDLK_r:
				{
					m_world->SetEye(Vector(0, 0, 0));
					break;
				}
				case SDLK_p:
				{
					const Vector &eye = m_world->GetEye();
					printf("Eye: %f, %f, %f\n", eye.m_x, eye.m_y, eye.m_z);
					break;
				}
				case SDLK_ESCAPE:
				{
					Stop();
					break;
				}
				case SDLK_f:
				{
					m_outputFPS = !m_outputFPS;
					break;
				}
			}
		}
	}
}

void RayTracer::RayTracer::DrawWorld()
{
	SDL_Surface *surface = SDL_GetWindowSurface(m_window);
	m_world->DrawWorld(surface);
	SDL_UpdateWindowSurface(m_window);
}

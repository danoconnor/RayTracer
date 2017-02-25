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
	std::thread test(&RayTracer::RayTracer::RunLoop, this); // Kick off a new thread that will handle user input and drawing the game world
	test.join();
}

void RayTracer::RayTracer::Stop()
{
	m_isRunning = false;
}

void RayTracer::RayTracer::AddObject(const WorldObject & object)
{
}

void RayTracer::RayTracer::AddSun(const LightSource & sun)
{
}

void RayTracer::RayTracer::AddPointLight(const LightSource & pointLight)
{
}

void RayTracer::RayTracer::RunLoop()
{
	while (m_isRunning)
	{
		ProcessUserInput();
		DrawWorld();
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
					printf("Eye: %f, %f, %f", eye.m_x, eye.m_y, eye.m_z);
					break;
				}
				case SDLK_ESCAPE:
				{
					Stop();
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

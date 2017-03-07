#include "stdafx.h"
#include "RayTracer.h"

namespace RT = RayTracer;

const float RT::RayTracer::Movement_Step = 0.2f;
const float RT::RayTracer::Look_Step = 0.2f;

RT::RayTracer::RayTracer()
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
	m_world->SetEye(Vector(-230, 350, 1000));
	m_world->SetForward(Vector(0.7f, -0.1f, -1.f));

	m_isRunning = false;
	m_outputFPS = true;
}

RT::RayTracer::~RayTracer()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	delete m_world;
}

void RT::RayTracer::Run()
{
	m_isRunning = true;
	RunLoop();
}

void RT::RayTracer::Stop()
{
	m_isRunning = false;
}

void RT::RayTracer::AddTriangle(const TrianglePlane *triangle)
{
	m_world->AddTriangle(triangle);
}

void RT::RayTracer::AddSphere(const Sphere *sphere)
{
	m_world->AddSphere(sphere);
}

void RT::RayTracer::AddRectangle(const RectangularPlane* rectangle)
{
	m_world->AddRectangle(rectangle);
}

void RT::RayTracer::AddSun(const LightSource* sun)
{
	m_world->AddSun(sun);
}

void RT::RayTracer::AddPointLight(const LightSource* pointLight)
{
	m_world->AddPointLight(pointLight);
}

void RT::RayTracer::Add3DObject(const std::string &filePath)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	std::string error = importer.GetErrorString();
	if (error.length() > 0)
	{
		printf("Error when trying to import %s.\nError string: %s", filePath.c_str(), error.c_str());
	}

	std::vector<const RT::TrianglePlane*> meshTriangles;
	if (scene != nullptr)
	{
		COLORREF triangleColor = 0xffffffff;
		aiMesh *mesh = scene->mMeshes[0];
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			if (face.mNumIndices == 3)
			{
				aiVector3D vertex1 = mesh->mVertices[face.mIndices[0]];
				aiVector3D vertex2 = mesh->mVertices[face.mIndices[1]];
				aiVector3D vertex3 = mesh->mVertices[face.mIndices[2]];

				RT::TrianglePlane *triangle = new RT::TrianglePlane(RT::Vector(vertex1.x, vertex1.y, vertex1.z),
					RT::Vector(vertex2.x, vertex2.y, vertex2.z),
					RT::Vector(vertex3.x, vertex3.y, vertex3.z),
					triangleColor,
					1.f);
				AddTriangle(triangle);
			}
		}
	}
}

void RT::RayTracer::RunLoop()
{
	float fps = 0;
	long begin = 0;
	long end = 0;

	bool drawWorld = true;
	while (m_isRunning)
	{
		begin = GetTickCount();

		ProcessUserInput();

		// We're only going to draw the world once but we need the loop to continue to process input so that the window doesn't appear to hang.
		if (drawWorld)
		{
			DrawWorld();
		}

		end = GetTickCount();

		if (begin != end)
		{
			fps = (1.f / (end - begin)) * 1000;
		}
		else
		{
			fps = 1000;
		}
		
		if (m_outputFPS && drawWorld)
		{
			printf("FPS: %f\n", fps);
			drawWorld = false;
		}
	}
}

void RT::RayTracer::ProcessUserInput()
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

void RT::RayTracer::DrawWorld()
{
	SDL_Surface *surface = SDL_GetWindowSurface(m_window);
	m_world->DrawWorld(surface);
	SDL_UpdateWindowSurface(m_window);
}

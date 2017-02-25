//#include "stdafx.h"
//#include "RayTracer.h"
//
//int _tmain(int argc, _TCHAR *argv[])
//{
//	RayTracer::RayTracer tracer;
//	
//	tracer.Run();
//
//	std::cin.get();
//
//	return 0;
//}

// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Vector.h"
#include "WorldObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "RectangularPlane.h"
#include "LightSource.h"
#include "TrianglePlane.h"

#include <iostream>
#include <time.h>
#include <thread>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#undef max // This needs to be undef'ed so that numeric_limits<T>::max will compile.
#define calculate_max(a,b) (((a)>(b))?(a):(b)) // Redefine "max" macro to be called "calculate_max" to avoid name collisions with numeric_limits<T>::max

#include <limits>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define SURFACE_PIXEL_SIZE 4
#define SURFACE_PITCH 2560

#define COLOR_DIVIDE_CONSTANT 255

#define MOVEMENT_STEP 0.2f

using namespace RayTracer;

std::vector<WorldObject *> m_worldObjects;
std::vector<LightSource *> m_suns;
std::vector<LightSource *> m_pointLights;

std::vector<cimg_library::CImg<unsigned char>> m_loadedTexures;

bool quit = false;

Vector eye(0, 0, 0);
Vector m_forward(0, 0, -1);
Vector m_right(1, 0, 0);
Vector m_up(0, 1, 0);

struct Collision
{
	Collision(WorldObject *obj, Vector point)
	{
		object = obj;
		collisionPoint = point;
	}

	WorldObject *object;
	Vector collisionPoint;
};

inline void SetSurfacePixel(SDL_Surface *surface, int x, int y, Uint8 red, Uint8 green, Uint8 blue)
{
	Uint8 *target = (Uint8 *) surface->pixels + y * SURFACE_PITCH + x * SURFACE_PIXEL_SIZE;
	*target = blue;
	*(target + 1) = green;
	*(target + 2) = red;
}

bool initWindow(SDL_Window *&window)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! Error %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("Window was not created properly. Error: %s", SDL_GetError());
		return false;
	}

	if (SURFACE_PITCH != SDL_GetWindowSurface(window)->pitch)
	{
		printf("Surface pitch macro is inaccurate");
		return false;
	}

	return true;
}

std::string GetArgument(const std::string &input, int position)
{
	int curPos = 0;

	std::string targetArg;

	for (char c : input)
	{
		if (c == ' ')
		{
			++curPos;
			if (curPos > position)
			{
				break;
			}
		}
		else if (curPos == position)
		{
			targetArg += c;
		}
	}

	return targetArg;
}

void ReadUserInput()
{
	while (!quit)
	{
		std::string command;
		std::cin >> command;

		if (command.size() > 0)
		{
			std::string commandType = GetArgument(command, 0);

			if (commandType == "plane")
			{
				float a = (float)atof(GetArgument(command, 1).c_str());
				float b = (float)atof(GetArgument(command, 2).c_str());
				float c = (float)atof(GetArgument(command, 3).c_str());
				float d = (float)atof(GetArgument(command, 4).c_str());

				int rValue = atoi(GetArgument(command, 5).c_str());
				int gValue = atoi(GetArgument(command, 6).c_str());
				int bValue = atoi(GetArgument(command, 7).c_str());

				Plane *plane = new Plane(a, b, c, d, RGB(rValue, gValue, bValue), 255);
				//worldObjects.push_back(plane); // Might cause bugs!
			}
			else if (commandType == "sphere")
			{
				float cX = (float)atof(GetArgument(command, 1).c_str());
				float cY = (float)atof(GetArgument(command, 2).c_str());
				float cZ = (float)atof(GetArgument(command, 3).c_str());

				float r = (float)atof(GetArgument(command, 4).c_str());

				int rValue = atoi(GetArgument(command, 5).c_str());
				int gValue = atoi(GetArgument(command, 6).c_str());
				int bValue = atoi(GetArgument(command, 7).c_str());

				Sphere *sphere = new Sphere(Vector(cX, cY, cZ), r, RGB(rValue, gValue, bValue), 255);
				//worldObjects.push_back(sphere); // Might cause bugs!
			}
			else if (commandType == "triangle")
			{

			}
			else if (commandType == "quit")
			{
				quit = true;
			}
		}
	}
}

bool SortByDistToEye(const Collision &c1, const Collision &c2)
{ 
	float c1DistToEye = abs(c1.collisionPoint.m_x - eye.m_x) + abs(c1.collisionPoint.m_y - eye.m_y) + abs(c1.collisionPoint.m_z - eye.m_z);
	float c2DistToEye = abs(c2.collisionPoint.m_x - eye.m_x) + abs(c2.collisionPoint.m_y - eye.m_y) + abs(c2.collisionPoint.m_z - eye.m_z);

	return c1DistToEye < c2DistToEye;
}

// Traces a ray from the window pixel positions x,y and returns the color that should be set at that location.
COLORREF TraceRay(int x, int y)
{
	float s = (2.f * x - WINDOW_WIDTH) / calculate_max(WINDOW_WIDTH, WINDOW_HEIGHT);
	float t = (WINDOW_HEIGHT - 2.f * y) / calculate_max(WINDOW_WIDTH, WINDOW_HEIGHT);

	Vector ray;
	ray.m_x = m_forward.m_x + s*m_right.m_x + t*m_up.m_x;
	ray.m_y = m_forward.m_y + s*m_right.m_y + t*m_up.m_y;
	ray.m_z = m_forward.m_z + s*m_right.m_z + t*m_up.m_z;

	ray.normalize();

	std::vector<Collision> collisions;

	for (WorldObject *obj : m_worldObjects)
	{
		float collisionDist;
		Vector cPoint;
		if (obj->CheckCollision(eye, ray, collisionDist, cPoint))
		{
			const Collision collision(obj, cPoint);
			collisions.push_back(collision);
		}
	}

	// Sort so that the closest points are at the front of the list
	std::sort(collisions.begin(), collisions.end(), &SortByDistToEye);

	float eyeRayAlpha = 1;

	Uint8 totalRed = 0;
	Uint8 totalGreen = 0;
	Uint8 totalBlue = 0;

	for (const Collision &collision : collisions)
	{
		Uint8 red = 0;
		Uint8 green = 0;
		Uint8 blue = 0;

		for (LightSource *pointlight : m_pointLights)
		{
			Vector lightDir = pointlight->GetPosorDir() - collision.collisionPoint;
			float xDiff = lightDir.m_x;
			lightDir.normalize();
			
			// Calculate T-value, not true distance
			float distToBulb = xDiff / lightDir.m_x;

			float lightRayAlpha = 1.f;
			for (WorldObject *obj : m_worldObjects)
			{
				if (obj != collision.object)
				{
					float d;
					Vector c;
					bool hasCollision = obj->CheckCollision(collision.collisionPoint, lightDir, d, c);
					if (hasCollision && d < distToBulb)
					{
						lightRayAlpha -= obj->GetAlpha();
						if (lightRayAlpha <= 0.f)
						{
							break;
						}
					}
				}
			}

			if (lightRayAlpha > 0.f)
			{
				Vector normal = collision.object->GetNormalAt(collision.collisionPoint, ray);
				float dot = normal.dot(lightDir);

				if (dot > 0)
				{
					COLORREF lightColor = pointlight->GetColor();
					COLORREF objectColor = collision.object->GetColorAt(collision.collisionPoint);
					
					int r = (int)(floor(GetRValue(lightColor) * GetRValue(objectColor) * dot * lightRayAlpha / (COLOR_DIVIDE_CONSTANT))) + red;
					int g = (int)(floor(GetGValue(lightColor) * GetGValue(objectColor) * dot * lightRayAlpha / (COLOR_DIVIDE_CONSTANT))) + green;
					int b = (int)(floor(GetBValue(lightColor) * GetBValue(objectColor) * dot * lightRayAlpha / (COLOR_DIVIDE_CONSTANT))) + blue;

					red = r > 255 ? 255 : r;
					green = g > 255 ? 255 : g;
					blue = b > 255 ? 255 : b;
				}
			}
		}

		for (LightSource *sun : m_suns)
		{
			Vector lightDir = sun->GetPosorDir();

			float sunRayAlpha = 1.f;

			for (WorldObject *obj : m_worldObjects)
			{
				if (obj != collision.object)
				{
					float d;
					Vector c;
					bool hasCollision = obj->CheckCollision(collision.collisionPoint, lightDir, d, c);
					if (hasCollision)
					{
						sunRayAlpha -= obj->GetAlpha();
						if (sunRayAlpha <= 0.f)
						{
							break;
						}
					}
				}
			}

			if (sunRayAlpha > 0.f)
			{
				Vector normal = collision.object->GetNormalAt(collision.collisionPoint, ray);
				float dot = normal.dot(lightDir);

				if (dot > 0)
				{
					COLORREF lightColor = sun->GetColor();
					COLORREF objectColor = collision.object->GetColorAt(collision.collisionPoint);

					int r = (int) (floor(GetRValue(lightColor) * GetRValue(objectColor) * dot * sunRayAlpha / (COLOR_DIVIDE_CONSTANT))) + red;
					int g = (int) (floor(GetGValue(lightColor) * GetGValue(objectColor) * dot * sunRayAlpha / (COLOR_DIVIDE_CONSTANT))) + green;
					int b = (int) (floor(GetBValue(lightColor) * GetBValue(objectColor) * dot * sunRayAlpha / (COLOR_DIVIDE_CONSTANT))) + blue;

					red = r > 255 ? 255 : r;
					green = g > 255 ? 255 : g;
					blue = b > 255 ? 255 : b;
				}
			}
		}

		int r = (int) (floor(totalRed + (red * collision.object->GetAlpha() * eyeRayAlpha)));
		int g = (int) (floor(totalGreen + (green * collision.object->GetAlpha() * eyeRayAlpha)));
		int b = (int) (floor(totalBlue + (blue * collision.object->GetAlpha() * eyeRayAlpha)));

		totalRed = r > 255 ? 255 : r;
		totalGreen = g > 255 ? 255 : g;
		totalBlue = b > 255 ? 255 : b;

		eyeRayAlpha -= collision.object->GetAlpha();
		if (eyeRayAlpha <= 0.f)
		{
			break;
		}
	}

	return RGB(totalRed, totalGreen, totalBlue);
}

void LoadTextures()
{
	cimg_library::CImg<unsigned char> worldTexture("Textures/logo.bmp");
	m_loadedTexures.push_back(worldTexture);

	cimg_library::CImg<unsigned char> globeTexture("Textures/earth.bmp");
	m_loadedTexures.push_back(globeTexture);

	cimg_library::CImg<unsigned char> brickTexture("Textures/brick.bmp");
	m_loadedTexures.push_back(brickTexture);
}

// Populate world with an initial set of objects for testing.
void LoadWorldObjects()
{
	Vector center(0, 0, -1);
	float radius = 0.3f;
	COLORREF c = RGB(0, 0, 0xff);
	Sphere *s = new Sphere(center, radius, m_loadedTexures[1], 1.f);
	//worldObjects.push_back(s);

	float c3 = 5;
	float d3 = 2;
	COLORREF backgroundPlaneColor = RGB(0xff, 0xff, 0xff);
	Plane *bPlane = new Plane(0, 0, c3, d3, m_loadedTexures[2], 1.f, m_loadedTexures[2].width() / 75.f, m_loadedTexures[2].height() / 75.f);
	//worldObjects.push_back(bPlane);

	Plane *bPlane2 = new Plane(0, 0, c3, d3, backgroundPlaneColor, 0.f);
	//worldObjects.push_back(bPlane2);

	//TrianglePlane *tP = new TrianglePlane(Vector(0, 0.5f, -1.5f), Vector(-0.5f, 0, -1.f), Vector(0.5f, 0, -1.f), m_loadedTexures[0], 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f);
	//worldObjects.push_back(tP);

	Sphere *sp = new Sphere(Vector(2.0f, 0, -4.f), 1.5f, RGB(0, 0, 0xff), 0.55f);
	m_worldObjects.push_back(sp);

	Sphere *sp2 = new Sphere(Vector(2.0f, 0, -8.f), 3.f, RGB(0xff, 0xff, 0xff), 1.f);
	m_worldObjects.push_back(sp2);

	Sphere *sp3 = new Sphere(Vector(3.0f, 0, -4.f), 1.5f, RGB(0, 0, 0xff), 0.25f);
	m_worldObjects.push_back(sp3);

	float a2 = 0.f;
	float b2 = 1.f;
	float c2 = 0.1f;
	float d2 = 0.5f;
	COLORREF planeColor = RGB(0x0, 0xff, 0x0);
	Plane *plane = new Plane(a2, b2, c2, d2, planeColor, 1.f);
	//worldObjects.push_back(plane);

	LightSource *pointlight = new LightSource(Vector(1, 0, 0));
	//pointLights.push_back(pointlight);

	LightSource *sun = new LightSource(Vector(1, 1, 1));
	//suns.push_back(sun);

	LightSource *sun2 = new LightSource(Vector(0, 0, 1));
	m_suns.push_back(sun2);

	LightSource *sun3 = new LightSource(Vector(0, 0, -1));
	m_suns.push_back(sun3);

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile("C:/Users/dpo3y/OneDrive/Documents/Visual Studio 2015/Projects/RayTracer3/TRex/T-Rex/T-Rex.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	auto error = importer.GetErrorString();

	if (scene != nullptr)
	{
		auto test = scene->mMeshes;
		auto mesh = test[0];
		auto materialIndex = mesh->mMaterialIndex;
		auto material = scene->mMaterials[materialIndex];
		auto face = mesh->mFaces[0];
		auto vert = mesh->mVertices[face.mIndices[0]];
	}
}

void ProcessUserInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
				case SDLK_UP:
				{
					printf("UP key pressed\n");

					float dy = MOVEMENT_STEP;
					float dz = MOVEMENT_STEP;

					if (m_forward.m_z >= 0)
					{
						dy = -dy;
					}

					if (m_forward.m_y <= 0)
					{
						dz = -dz;
					}

					m_forward.m_y += dy;
					m_forward.m_z += dz;

					m_right = Vector::Cross(m_forward, m_up);
					m_right.normalize();

					m_up = Vector::Cross(m_right, m_forward);
					m_up.normalize();

					m_forward.normalize();

					break;
				}
				case SDLK_DOWN:
				{
					printf("DOWN key pressed\n");

					float dy = MOVEMENT_STEP;
					float dz = MOVEMENT_STEP;

					if (m_forward.m_z <= 0)
					{
						dy = -dy;
					}

					if (m_forward.m_y >= 0)
					{
						dz = -dz;
					}

					m_forward.m_y += dy;
					m_forward.m_z += dz;

					m_right = Vector::Cross(m_forward, m_up);
					m_right.normalize();

					m_up = Vector::Cross(m_right, m_forward);
					m_up.normalize();

					m_forward.normalize();

					break;
				}
				case SDLK_RIGHT:
				{
					printf("RIGHT key pressed\n");

					float dx = MOVEMENT_STEP;
					float dz = MOVEMENT_STEP;

					if (m_forward.m_z >= 0)
					{
						dx = -dx;
					}

					if (m_forward.m_x <= 0)
					{
						dz = -dz;
					}

					m_forward.m_x += dx;
					m_forward.m_z += dz;

					m_right = Vector::Cross(m_forward, m_up);
					m_right.normalize();

					m_up = Vector::Cross(m_right, m_forward);
					m_up.normalize();

					m_forward.normalize();

					break;
				}
				case SDLK_LEFT:
				{
					printf("LEFT key pressed\n");

					float dx = MOVEMENT_STEP;
					float dz = MOVEMENT_STEP;

					if (m_forward.m_z <= 0)
					{
						dx = -dx;
					}

					if (m_forward.m_x >= 0)
					{
						dz = -dz;
					}

					m_forward.m_x += dx;
					m_forward.m_z += dz;

					m_right = Vector::Cross(m_forward, m_up);
					m_right.normalize();

					m_up = Vector::Cross(m_right, m_forward);
					m_up.normalize();

					m_forward.normalize();

					break;
				}
				case SDLK_q:
				{
					printf("Q key pressed\n");

					Vector upCopy = m_up;
					upCopy.scalarMult(-MOVEMENT_STEP);

					eye = eye + upCopy;

					break;
				}
				case SDLK_e:
				{
					printf("E key pressed\n");

					Vector upCopy = m_up;
					upCopy.scalarMult(MOVEMENT_STEP);

					eye = eye + upCopy;

					break;
				}
				case SDLK_w:
				{
					printf("W key pressed\n");

					Vector forwardCopy = m_forward;
					forwardCopy.scalarMult(MOVEMENT_STEP);

					eye = eye + forwardCopy;

					break;
				}
				case SDLK_s:
				{
					printf("S key pressed\n");

					Vector forwardCopy = m_forward;
					forwardCopy.scalarMult(-MOVEMENT_STEP);

					eye = eye + forwardCopy;

					break;
				}
				case SDLK_d:
				{
					printf("D key pressed\n");

					Vector rightCopy = m_right;
					rightCopy.scalarMult(MOVEMENT_STEP);

					eye = eye + rightCopy;

					break;
				}
				case SDLK_a:
				{
					printf("A key pressed\n");

					Vector rightCopy = m_right;
					rightCopy.scalarMult(-MOVEMENT_STEP);

					eye = eye + rightCopy;

					break;
				}
				case SDLK_r:
				{
					printf("R key pressed\n");

					eye = Vector(0, 0, 0);
					m_forward = Vector(0, 0, -1);
					m_right = Vector(1, 0, 0);
					m_up = Vector(0, 1, 0);

					break;
				}
				case SDLK_p:
				{
					printf("Eye Position: %f, %f, %f", eye.m_x, eye.m_y, eye.m_z);
					break;
				}
				case SDLK_ESCAPE:
				{
					quit = true;
					break;
				}
			}
		}
	}
}

void DrawWorld(SDL_Window *window, SDL_Surface *surface)
{
	surface = SDL_GetWindowSurface(window);

	for (int i = 0; i < surface->h; ++i)
	{
		for (int j = 0; j < surface->w; ++j)
		{
			COLORREF color = TraceRay(j, i);
			SetSurfacePixel(surface, j, i, GetRValue(color), GetGValue(color), GetBValue(color));
		}
	}

	SDL_UpdateWindowSurface(window);
}

int _tmain(int argc, _TCHAR *argv[]) 
{
	SDL_Window *window = nullptr;
	SDL_Surface *surface = nullptr;
	
	if (!initWindow(window))
	{
		return -1;
	}

	LoadTextures();
	LoadWorldObjects();

	float fps = 0;
	
	//std::thread userInputThread(ReadUserInput);

	long begin = GetTickCount();

	DrawWorld(window, surface);

	long end = GetTickCount();
	if (begin != end)
	{
		fps = (1.f / (end - begin)) * 1000;
	}
	else
	{
		fps = 1000;
	}

	printf("FPS: %f\n", fps);

	while (!quit)
	{
		long begin = GetTickCount();
		
		ProcessUserInput();
		DrawWorld(window, surface);

		long end = GetTickCount();
		if (begin != end)
		{
			fps = (1.f / (end - begin)) * 1000;
		}
		else
		{
			fps = 1000;
		}

		printf("FPS: %f\n", fps);
	}

	for (auto object : m_worldObjects)
	{
		delete object;
	}

	//userInputThread.join();
	
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

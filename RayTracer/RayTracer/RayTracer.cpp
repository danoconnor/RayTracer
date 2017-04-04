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
	//m_world->SetEye(Vector(-230, 350, 1000));
	//m_world->SetForward(Vector(-1, 0, -1.f));

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
	Add3DObject(filePath, Vector(0, 0, 0), 0, 0, 0);
}

void RT::RayTracer::Add3DObject(const std::string &filePath, const Vector &translation, float rotationX, float rotationY, float rotationZ)
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
		std::vector<RT::TrianglePlane> objectTriangles;

		float avgX = 0;
		float avgY = 0;
		float avgZ = 0;
		unsigned int numTriangleVertices = mesh->mNumFaces * 3;

		// Iterate through all the object triangles. Calculate the center of the object (for rotation purposes) and move the raw object data into our custom Triangle class.
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			if (face.mNumIndices == 3)
			{
				aiVector3D vertex1 = mesh->mVertices[face.mIndices[0]];
				aiVector3D vertex2 = mesh->mVertices[face.mIndices[1]];
				aiVector3D vertex3 = mesh->mVertices[face.mIndices[2]];

				avgX += (vertex1.x + vertex2.x + vertex3.x);
				avgY += (vertex1.y + vertex2.y + vertex3.y);
				avgZ += (vertex1.z + vertex2.z + vertex3.z);

				objectTriangles.push_back(RT::TrianglePlane(RT::Vector(vertex1.x, vertex1.y, vertex1.z),
					RT::Vector(vertex2.x, vertex2.y, vertex2.z),
					RT::Vector(vertex3.x, vertex3.y, vertex3.z),
					triangleColor,
					1.f,
					1.f,
					0.f));
			}
		}

		// Find the center of the object
		avgX /= numTriangleVertices;
		avgY /= numTriangleVertices;
		avgZ /= numTriangleVertices;

		Vector shapeCenter(avgX, avgY, avgZ);

		const float PI = 3.14159265358979323846f;
		const float oneEighty = 180.f;

		// Convert the rotations in degrees to radians
		float rotXRads = (rotationX * PI / oneEighty);
		float rotYRads = (rotationY * PI / oneEighty);
		float rotZRads = (rotationZ * PI / oneEighty);

		// Store off the rotation matrix
		Vector rotationMatrixR1(
			cos(rotYRads)*cos(rotZRads),
			cos(rotXRads)*sin(rotZRads) + sin(rotXRads)*sin(rotYRads)*cos(rotZRads),
			sin(rotXRads)*sin(rotZRads) - cos(rotXRads)*sin(rotYRads)*cos(rotZRads));

		Vector rotationMatrixR2(
			-1 * cos(rotYRads)*sin(rotZRads),
			cos(rotXRads)*cos(rotZRads) - sin(rotXRads)*sin(rotYRads)*sin(rotZRads),
			sin(rotXRads)*cos(rotZRads) + cos(rotXRads)*sin(rotYRads)*sin(rotZRads));

		Vector rotationMatrixR3(
			sin(rotYRads),
			-1*sin(rotXRads)*cos(rotYRads),
			cos(rotXRads)*cos(rotYRads));

		// Transform/rotate the triangles and add them to the world
		for (RT::TrianglePlane &triangle : objectTriangles)
		{
			std::vector<RT::Vector> trianglePoints = triangle.GetPoints();
			assert(trianglePoints.size() == 3);

			Vector p1 = trianglePoints[0];
			Vector p2 = trianglePoints[1];
			Vector p3 = trianglePoints[2];

			// Translate the shape to the origin for the rotation so we can rotate it around the x, y, and z axes
			p1 = p1 - shapeCenter;
			p2 = p2 - shapeCenter;
			p3 = p3 - shapeCenter;

			// Rotate the translated points
			p1 = Vector(
				rotationMatrixR1.m_x*p1.m_x + rotationMatrixR1.m_y*p1.m_y + rotationMatrixR1.m_z*p1.m_z,
				rotationMatrixR2.m_x*p1.m_x + rotationMatrixR2.m_y*p1.m_y + rotationMatrixR2.m_z*p1.m_z,
				rotationMatrixR3.m_x*p1.m_x + rotationMatrixR3.m_y*p1.m_y + rotationMatrixR3.m_z*p1.m_z);
			p2 = Vector(
				rotationMatrixR1.m_x*p2.m_x + rotationMatrixR1.m_y*p2.m_y + rotationMatrixR1.m_z*p2.m_z,
				rotationMatrixR2.m_x*p2.m_x + rotationMatrixR2.m_y*p2.m_y + rotationMatrixR2.m_z*p2.m_z,
				rotationMatrixR3.m_x*p2.m_x + rotationMatrixR3.m_y*p2.m_y + rotationMatrixR3.m_z*p2.m_z);
			p3 = Vector(
				rotationMatrixR1.m_x*p3.m_x + rotationMatrixR1.m_y*p3.m_y + rotationMatrixR1.m_z*p3.m_z,
				rotationMatrixR2.m_x*p3.m_x + rotationMatrixR2.m_y*p3.m_y + rotationMatrixR2.m_z*p3.m_z,
				rotationMatrixR3.m_x*p3.m_x + rotationMatrixR3.m_y*p3.m_y + rotationMatrixR3.m_z*p3.m_z);

			// Undo the origin translation now that the triangle has been rotated
			p1 = p1 + shapeCenter;
			p2 = p2 + shapeCenter;
			p3 = p3 + shapeCenter;

			// Apply the translation to the rotated points
			p1 = p1 + translation;
			p2 = p2 + translation;
			p3 = p3 + translation;
			
			AddTriangle(new RT::TrianglePlane(
				p1,
				p2,
				p3,
				triangleColor,
				triangle.GetAlpha(),
				triangle.GetRefraction(),
				triangle.GetReflectivity()
			));
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

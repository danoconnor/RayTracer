#include "stdafx.h"
#include "RayTracer.h"

#include "Vector.h"
#include "WorldObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "RectangularPlane.h"
#include "TrianglePlane.h"

std::vector<cimg_library::CImg<unsigned char>> _loadedTexures;

void LoadTextures()
{
	cimg_library::CImg<unsigned char> worldTexture("Textures/logo.bmp");
	_loadedTexures.push_back(worldTexture);

	cimg_library::CImg<unsigned char> globeTexture("Textures/earth.bmp");
	_loadedTexures.push_back(globeTexture);

	cimg_library::CImg<unsigned char> brickTexture("Textures/brick.bmp");
	_loadedTexures.push_back(brickTexture);
}

void AddTestObjectsToWorld(RayTracer::RayTracer& tracer)
{
	RayTracer::Vector center(0, 0, -1);
	float radius = 0.3f;
	COLORREF c = RGB(0, 0, 0xff);
	RayTracer::Sphere s(center, radius, _loadedTexures[1], 1.f);
	//worldObjects.push_back(s);
	
	float c3 = 5;
	float d3 = 2;
	COLORREF backgroundPlaneColor = RGB(0xff, 0xff, 0xff);
	RayTracer::Plane bPlane(0, 0, c3, d3, _loadedTexures[2], 1.f, _loadedTexures[2].width() / 75.f, _loadedTexures[2].height() / 75.f);
	//worldObjects.push_back(bPlane);
	
	RayTracer::Plane bPlane2(0, 0, c3, d3, backgroundPlaneColor, 0.f);
	//worldObjects.push_back(bPlane2);
	
	//TrianglePlane *tP = new TrianglePlane(Vector(0, 0.5f, -1.5f), Vector(-0.5f, 0, -1.f), Vector(0.5f, 0, -1.f), m_loadedTexures[0], 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f);
	//worldObjects.push_back(tP);
	
	RayTracer::Sphere *sp = new RayTracer::Sphere(RayTracer::Vector(2.0f, 0, -4.f), 1.5f, RGB(0, 0, 0xff), 0.55f);
	tracer.AddObject(sp);
	
	RayTracer::Sphere *sp2 = new RayTracer::Sphere(RayTracer::Vector(2.0f, 0, -8.f), 3.f, RGB(0xff, 0xff, 0xff), 1.f);
	tracer.AddObject(sp2);
	
	RayTracer::Sphere *sp3 = new RayTracer::Sphere(RayTracer::Vector(3.0f, 0, -4.f), 1.5f, RGB(0, 0, 0xff), 0.25f);
	tracer.AddObject(sp3);
	
	float a2 = 0.f;
	float b2 = 1.f;
	float c2 = 0.1f;
	float d2 = 0.5f;
	COLORREF planeColor = RGB(0x0, 0xff, 0x0);
	RayTracer::Plane plane(a2, b2, c2, d2, planeColor, 1.f);
	//worldObjects.push_back(plane);
	
	RayTracer::LightSource pointlight(RayTracer::Vector(1, 0, 0));
	//pointLights.push_back(pointlight);
	
	RayTracer::LightSource sun(RayTracer::Vector(1, 1, 1));
	//suns.push_back(sun);
	
	RayTracer::LightSource *sun2 = new RayTracer::LightSource(RayTracer::Vector(0, 0, 1));
	tracer.AddSun(sun2);
	
	RayTracer::LightSource *sun3 = new RayTracer::LightSource(RayTracer::Vector(0, 0, -1));
	tracer.AddSun(sun3);
	
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile("Textures/TRex.stl",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	auto error = importer.GetErrorString();
	
	// Testing
	float xMin = 9999999;
	float xMax = -9999999;
	float yMin = 9999999;
	float yMax = -9999999;
	float zMin = 9999999;
	float zMax = -9999999;

	std::vector<const RayTracer::TrianglePlane*> meshTriangles;
	if (scene != nullptr)
	{
		COLORREF triangleColor = 0x00ffffff;
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

				// Testing
				if (vertex1.x > xMax)
				{
					xMax = vertex1.x;
				}
				else if (vertex1.x < xMin)
				{
					xMin = vertex1.x;
				}
				
				if (vertex1.y > yMax)
				{
					yMax = vertex1.y;
				}
				else if (vertex1.y < yMin)
				{
					yMin = vertex1.y;
				}

				if (vertex1.z > zMax)
				{
					zMax = vertex1.z;
				}
				else if (vertex1.z < zMin)
				{
					zMin = vertex1.z;
				}

				if (vertex2.x > xMax)
				{
					xMax = vertex2.x;
				}
				else if (vertex2.x < xMin)
				{
					xMin = vertex2.x;
				}

				if (vertex2.y > yMax)
				{
					yMax = vertex2.y;
				}
				else if (vertex2.y < yMin)
				{
					yMin = vertex2.y;
				}

				if (vertex2.z > zMax)
				{
					zMax = vertex2.z;
				}
				else if (vertex2.z < zMin)
				{
					zMin = vertex2.z;
				}

				if (vertex3.x > xMax)
				{
					xMax = vertex3.x;
				}
				else if (vertex3.x < xMin)
				{
					xMin = vertex3.x;
				}

				if (vertex3.y > yMax)
				{
					yMax = vertex3.y;
				}
				else if (vertex3.y < yMin)
				{
					yMin = vertex3.y;
				}

				if (vertex3.z > zMax)
				{
					zMax = vertex3.z;
				}
				else if (vertex3.z < zMin)
				{
					zMin = vertex3.z;
				}

				RayTracer::TrianglePlane *triangle = new RayTracer::TrianglePlane(RayTracer::Vector(vertex1.x, vertex1.y, vertex1.z), 
					RayTracer::Vector(vertex2.x, vertex2.y, vertex2.z), 
					RayTracer::Vector(vertex3.x, vertex3.y, vertex3.z),
					triangleColor,
					1.f);
				tracer.AddObject(triangle);
			}
		}
	}

	printf("Mesh boundaries: X: [%f, %f]  Y: [%f, %f]  Z: [%f, %f]", xMin, xMax, yMin, yMax, zMin, zMax);
}

int _tmain(int argc, _TCHAR *argv[])
{
	RayTracer::RayTracer tracer;

	LoadTextures();
	AddTestObjectsToWorld(tracer);
	
	tracer.Run();

	std::cin.get();

	return 0;
}
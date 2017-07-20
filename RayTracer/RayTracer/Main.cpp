#include "stdafx.h"
#include "RayTracer.h"

#include "Vector.h"
#include "Sphere.h"
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
	//RayTracer::Sphere s(center, radius, _loadedTexures[1], 1.f);
	//worldObjects.push_back(s);
	
	RayTracer::Vector p1(-200, 0, -200);
	RayTracer::Vector p2(-200, 0, 1100);
	RayTracer::Vector p3(200, 0, -200);
	RayTracer::Vector p4(200, 0, 1100);
	RayTracer::RectangularPlane *plane = new RayTracer::RectangularPlane(p1, p2, p3, p4, _loadedTexures[2], 1.f, 1.f, 0.f, _loadedTexures[2].width(), _loadedTexures[2].height());
	//tracer.AddRectangle(plane);
	
	RayTracer::Vector p5(-200, -200, -20);
	RayTracer::Vector p6(-200, 200, -20);
	RayTracer::Vector p7(200, -200, -20);
	RayTracer::Vector p8(200, 200, -20);
	COLORREF backgroundPlaneColor = RGB(0, 0xff, 0);
	RayTracer::RectangularPlane *plane2 = new RayTracer::RectangularPlane(p5, p6, p7, p8, backgroundPlaneColor, 1.f, 1.f, 0);
	//tracer.AddRectangle(plane2);

	RayTracer::Vector p9(-10, 10, -5);
	RayTracer::Vector p10(10, 10, -5);
	RayTracer::Vector p11(-10, -10, -5);
	RayTracer::Vector p12(10, -10, -5);
	COLORREF backgroundPlaneColor2 = RGB(0xff, 0xff, 0xff);
	RayTracer::RectangularPlane *plane3 = new RayTracer::RectangularPlane(p9, p10, p11, p12, backgroundPlaneColor2, 1.f, 1.f, 1.f);
	//tracer.AddRectangle(plane3);

	RayTracer::Vector p13(0, 10, 1);
	RayTracer::Vector p14(0, -10, 1);
	RayTracer::RectangularPlane *plane4 = new RayTracer::RectangularPlane(p13, p14, p10, p12, backgroundPlaneColor2, 1.f, 1.f, 1.f);
	//tracer.AddRectangle(plane4);

	RayTracer::RectangularPlane *plane5 = new RayTracer::RectangularPlane(p13, p14, p9, p11, backgroundPlaneColor2, 1.f, 1.f, 1.f);
	//tracer.AddRectangle(plane5);

	//RayTracer::RectangularPlane bPlane2(0, 0, c3, d3, backgroundPlaneColor, 0.f);
	//worldObjects.push_back(bPlane2);
	
	//TrianglePlane *tP = new TrianglePlane(Vector(0, 0.5f, -1.5f), Vector(-0.5f, 0, -1.f), Vector(0.5f, 0, -1.f), m_loadedTexures[0], 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f);
	//worldObjects.push_back(tP);
	
	RayTracer::Sphere *sp = new RayTracer::Sphere(RayTracer::Vector(3, 0, -6), 1.5f, RGB(0, 0, 0xff), 0.99f, 1.f, 0);
	//tracer.AddSphere(sp);
	
	RayTracer::Sphere *sp2 = new RayTracer::Sphere(RayTracer::Vector(1.0f, 0, -3.f), 0.5f, RGB(0, 0, 0xff), 1.f, 1.f, 0);
	//tracer.AddSphere(sp2);
	
	RayTracer::Sphere *sp3 = new RayTracer::Sphere(RayTracer::Vector(-1.0f, 0, -3.f), .5f, RGB(0, 0xff, 0), 1.f, 1.f, 0);
	//tracer.AddSphere(sp3);

	RayTracer::Sphere *sp4 = new RayTracer::Sphere(RayTracer::Vector(14, 258, 740), 100, RGB(0xff, 0, 0), 1.f, 1.f, 0);
	//tracer.AddSphere(sp4);
	
	float a2 = 0.f;
	float b2 = 1.f;
	float c2 = 0.1f;
	float d2 = 0.5f;
	COLORREF planeColor = RGB(0x0, 0xff, 0x0);
	//RayTracer::Plane plane(a2, b2, c2, d2, planeColor, 1.f);
	//worldObjects.push_back(plane);
	
	RayTracer::LightSource *pointlight = new RayTracer::LightSource(RayTracer::Vector(0, 0, -1), RGB(0xaa, 0xaa, 0xaa));
	//tracer.AddPointLight(pointlight);

	RayTracer::LightSource *pointlight2 = new RayTracer::LightSource(RayTracer::Vector(0, 0, -4.5));
	//tracer.AddPointLight(pointlight2);

	RayTracer::LightSource *pointlight3 = new RayTracer::LightSource(RayTracer::Vector(4.5, 0, -2));
	//tracer.AddPointLight(pointlight3);

	RayTracer::LightSource *pointlight4 = new RayTracer::LightSource(RayTracer::Vector(-4.5, 0, -2));
	//tracer.AddPointLight(pointlight4);
	
	RayTracer::LightSource *sun = new RayTracer::LightSource(RayTracer::Vector(-1, 1, 0), RGB(0xaa, 0xaa, 0xaa));
	tracer.AddSun(sun);
	
	RayTracer::LightSource *sun2 = new RayTracer::LightSource(RayTracer::Vector(0, -1, 0), RGB(0x30, 0x30, 0x30));
	tracer.AddSun(sun2);
	
	RayTracer::LightSource *sun3 = new RayTracer::LightSource(RayTracer::Vector(0, 0, 1), RGB(0xff, 0xff, 0xff));
	//tracer.AddSun(sun3);

	RayTracer::LightSource *sun4 = new RayTracer::LightSource(RayTracer::Vector(-1, 1, 0), RGB(0xbb, 0xbb, 0xbb));
	//tracer.AddSun(sun4);
	
	//tracer.Add3DObject("Textures/Tree_V10_Final.obj", RayTracer::Vector(0, 0, 0), 90, 0, 0, 5);
	//tracer.Add3DObject("Textures/trex.stl", RayTracer::Vector(0, 0, 0), 90, 0, 0, 3);
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
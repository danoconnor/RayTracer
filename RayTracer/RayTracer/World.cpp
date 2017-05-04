#include "stdafx.h"
#include "World.h"

#define NUM_RAY_TRACE_THREADS 8
#define MAX_REFLECTION_RECURSION 3
#define ZERO_WITH_MARGIN_OF_ERROR 0.001

namespace RayTracer
{
	World::World() : m_eye(0, 0, 0), m_forward(0, 0, -1), m_right(1, 0, 0), m_up(0, 1, 0)
	{
	}

	World::~World()
	{
		for (const TrianglePlane *object : m_triangles)
		{
			delete object;
		}

		for (const Sphere *object : m_spheres)
		{
			delete object;
		}

		for (const RectangularPlane *object : m_rectangles)
		{
			delete object;
		}

		for (const LightSource *sun : m_suns)
		{
			delete sun;
		}

		for (const LightSource *pointLight : m_pointLights)
		{
			delete pointLight;
		}
	}

	void World::DrawWorld(SDL_Surface *surface)
	{
		std::thread threads[NUM_RAY_TRACE_THREADS];

		int surfaceHeight = surface->h;
		int rowsPerThread = (int)floor(surfaceHeight / NUM_RAY_TRACE_THREADS);

		// Start the threads, each of which will draw a section of the rows in the window
		for (int i = 0; i < NUM_RAY_TRACE_THREADS; ++i)
		{
			int beginRow = (i * rowsPerThread);
			int endRow = min((i + 1) * rowsPerThread, surfaceHeight);

			threads[i] = std::thread(&World::DrawWorldSubset, this, surface, beginRow, endRow);
		}

		// Wait for each thread to finish executing.
		for (int i = 0; i < NUM_RAY_TRACE_THREADS; ++i)
		{
			threads[i].join();
		}
	}

	void World::AddTriangle(const TrianglePlane *object)
	{
		m_triangles.push_back(object);
	}

	void World::AddSphere(const Sphere *object)
	{
		m_spheres.push_back(object);
	}

	void World::AddRectangle(const RectangularPlane *object)
	{
		m_rectangles.push_back(object);
	}

	void World::AddSun(const LightSource *sun)
	{
		m_suns.push_back(sun);
	}

	void World::AddPointLight(const LightSource *light)
	{
		m_pointLights.push_back(light);
	}

	void World::StepForward(float amount)
	{
		Vector forwardCopy = m_forward;
		forwardCopy.scalarMult(amount);

		SetEye(m_eye + forwardCopy);
	}

	void World::StepBackward(float amount)
	{
		Vector forwardCopy = m_forward;
		forwardCopy.scalarMult(-1 * amount);

		SetEye(m_eye + forwardCopy);
	}

	void World::StepRight(float amount)
	{
		Vector rightCopy = m_right;
		rightCopy.scalarMult(amount);

		SetEye(m_eye + rightCopy);
	}

	void World::StepLeft(float amount)
	{
		Vector rightCopy = m_right;
		rightCopy.scalarMult(-1 * amount);

		SetEye(m_eye + rightCopy);
	}

	void World::LookUp(float angle)
	{
		float dy = angle;
		float dz = angle;

		if (m_forward.m_z >= 0)
		{
			dy = -dy;
		}

		if (m_forward.m_y <= 0)
		{
			dz = -dz;
		}
		
		SetForward(Vector(m_forward.m_x, m_forward.m_y + dy, m_forward.m_z + dz));
	}

	void World::LookDown(float angle)
	{
		float dy = angle;
		float dz = angle;

		if (m_forward.m_z <= 0)
		{
			dy = -dy;
		}

		if (m_forward.m_y >= 0)
		{
			dz = -dz;
		}

		SetForward(Vector(m_forward.m_x, m_forward.m_y + dy, m_forward.m_z + dz));
	}

	void World::LookLeft(float angle)
	{
		float dx = angle;
		float dz = angle;

		if (m_forward.m_z <= 0)
		{
			dx = -dx;
		}

		if (m_forward.m_x >= 0)
		{
			dz = -dz;
		}

		SetForward(Vector(m_forward.m_x + dx, m_forward.m_y, m_forward.m_z + dz));
	}

	void World::LookRight(float angle)
	{
		float dx = angle;
		float dz = angle;

		if (m_forward.m_z >= 0)
		{
			dx = -dx;
		}

		if (m_forward.m_x <= 0)
		{
			dz = -dz;
		}

		SetForward(Vector(m_forward.m_x + dx, m_forward.m_y, m_forward.m_z + dz));
	}

	void World::MoveUp(float amount)
	{
		Vector upCopy = m_up;
		upCopy.scalarMult(amount);

		SetEye(m_eye + upCopy);
	}

	void World::MoveDown(float amount)
	{
		Vector upCopy = m_up;
		upCopy.scalarMult(-1 * amount);

		SetEye(m_eye + upCopy);
	}

	void World::SetEye(const Vector &newEye)
	{
		m_eye = newEye;
	}

	const Vector& World::GetEye()
	{
		return m_eye;
	}

	void World::SetForward(const Vector &newForward)
	{
		m_forward = newForward;

		m_right = Vector::Cross(m_forward, m_up);
		m_right.normalize();

		m_up = Vector::Cross(m_right, m_forward);
		m_up.normalize();

		m_forward.normalize();
	}

	const Vector& World::GetForward()
	{
		return m_forward;
	}

	const Vector& World::GetUp()
	{
		return m_up;
	}

	const Vector& World::GetRight()
	{
		return m_right;
	}

	// TODO - originObject is currently not being used but could be useful when we implement refraction so I'll leave it for now.
	COLORREF World::TraceRay(const Vector &rayOrigin, const Vector &ray, const void *originObject, float eyeRayAlpha, Uint8 reflectionRecursion)
	{
		// Don't want to get stuck in an infinite loop of reflection, so break once we get to a certain depth.
		if (reflectionRecursion >= MAX_REFLECTION_RECURSION)
		{
			return RGB(0, 0, 0);
		}

		// Check to see if the ray collides with any of our triangles, spheres, or rectangles.
		Collision collision;
		collision.distance = std::numeric_limits<float>::infinity();

		bool hasCollision = GetClosestCollision(rayOrigin, m_triangles, originObject, ray, collision);
		hasCollision |= GetClosestCollision(rayOrigin, m_spheres, originObject, ray, collision);
		hasCollision |= GetClosestCollision(rayOrigin, m_rectangles, originObject, ray, collision);

		if (!hasCollision)
		{
			return RGB(0, 0, 0);
		}
		
		// These need to be uint32 for now to handle any overflow while calculating the color. They will eventually be fit into uint8's for the final return value.
		unsigned int objectColorRed = GetRValue(collision.objectColor);
		unsigned int objectColorGreen = GetGValue(collision.objectColor);
		unsigned int objectColorBlue = GetBValue(collision.objectColor);

		// If this object is not fully opaque, then we need to shoot another ray to see what's behind it
		if (collision.objectAlpha < 1.f)
		{
			// TODO - calculate refraction direction
			float newEyeRayAlpha = eyeRayAlpha - collision.objectAlpha;
			if (newEyeRayAlpha > 0.f)
			{
				COLORREF transparentColor = TraceRay(collision.collisionPoint, ray, collision.object, newEyeRayAlpha, reflectionRecursion);

				objectColorRed = (unsigned int)floor((objectColorRed * collision.objectAlpha) + (GetRValue(transparentColor) * (1 - collision.objectAlpha)));
				objectColorGreen = (unsigned int)floor((objectColorGreen * collision.objectAlpha) + (GetGValue(transparentColor) * (1 - collision.objectAlpha)));
				objectColorBlue = (unsigned int)floor((objectColorBlue * collision.objectAlpha) + (GetBValue(transparentColor) * (1 - collision.objectAlpha)));
			}
		}

		// If this object is reflective, then we need to get the reflected color
		if (collision.objectReflectivity > 0.f)
		{
			// Calculate the reflection vector
			// reflectionVector = ray - 2*(ray dot normal)*normal
			Vector twoRayProjN = collision.objectNormal;
			twoRayProjN.scalarMult(2 * Vector::Dot(ray, collision.objectNormal));
			Vector reflectionVector = ray - twoRayProjN;

			COLORREF reflectedColor = TraceRay(collision.collisionPoint, reflectionVector, collision.object, 1.f, reflectionRecursion + 1);

			objectColorRed = (unsigned int)floor(((1 - collision.objectReflectivity) * objectColorRed) + (collision.objectReflectivity * GetRValue(reflectedColor)));
			objectColorGreen = (unsigned int)floor(((1 - collision.objectReflectivity) * objectColorGreen) + (collision.objectReflectivity * GetGValue(reflectedColor)));
			objectColorBlue = (unsigned int)floor(((1 - collision.objectReflectivity) * objectColorBlue) + (collision.objectReflectivity * GetBValue(reflectedColor)));
		}

		// Forward declare these so we don't have to reallocate the memory on every iteration of the loops
		float dot;
		float xDiff;
		float distToBulb;
		float lightRayAlpha;
		Vector lightDir;
		COLORREF lightColor;
		const Vector& normal = collision.objectNormal;

		// These represent the final RGB values, taking into consideration the lighting.
		// They need to be uint32 for now to handle any potential overflow, but will be fit into uint8's for the final return value.
		unsigned int tempRed = 0;
		unsigned int tempGreen = 0;
		unsigned int tempBlue = 0;
		
		for (const LightSource *pointlight : m_pointLights)
		{
			lightDir = pointlight->GetPosorDir() - collision.collisionPoint;
			xDiff = lightDir.m_x;
			lightDir.normalize();

			// Calculate T-value, not true distance
			distToBulb = xDiff / lightDir.m_x;

			lightRayAlpha = 1.f;
			TraceRayFromCollisionToPointLight(m_triangles, collision, lightDir, distToBulb, lightRayAlpha);
			TraceRayFromCollisionToPointLight(m_spheres, collision, lightDir, distToBulb, lightRayAlpha);
			TraceRayFromCollisionToPointLight(m_rectangles, collision, lightDir, distToBulb, lightRayAlpha);

			if (lightRayAlpha > 0.f)
			{
				dot = normal.dot(lightDir);

				if (dot > 0)
				{
					lightColor = pointlight->GetColor();

					tempRed += (int)(floor(GetRValue(lightColor) * objectColorRed * dot * lightRayAlpha / Color_Divide_Constant));
					tempGreen += (int)(floor(GetGValue(lightColor) * objectColorGreen * dot * lightRayAlpha / Color_Divide_Constant));
					tempBlue += (int)(floor(GetBValue(lightColor) * objectColorBlue * dot * lightRayAlpha / Color_Divide_Constant));
				}
			}
		}

		for (const LightSource *sun : m_suns)
		{
			const Vector& sunDir = sun->GetPosorDir();

			lightRayAlpha = 1.f;
			TraceRayFromCollisionToSun(m_triangles, collision, sunDir, lightRayAlpha);
			TraceRayFromCollisionToSun(m_spheres, collision, sunDir, lightRayAlpha);
			TraceRayFromCollisionToSun(m_rectangles, collision, sunDir, lightRayAlpha);

			if (lightRayAlpha > 0.f)
			{
				float dot = collision.objectNormal.dot(sunDir);

				if (dot > 0)
				{
					lightColor = sun->GetColor();

					tempRed += (int)(floor(GetRValue(lightColor) * objectColorRed * dot * lightRayAlpha / Color_Divide_Constant));
					tempGreen += (int)(floor(GetGValue(lightColor) * objectColorGreen * dot * lightRayAlpha / Color_Divide_Constant));
					tempBlue += (int)(floor(GetBValue(lightColor) * objectColorBlue * dot * lightRayAlpha / Color_Divide_Constant));
				}
			}
		}

		// TODO - trace from collision point to all available reflective surfaces (minimum distance point)
		// For each reflective surface collision, trace from the reflective surface to all light sources
		// If the light source is unblocked, make a copy of it rotated 180 degrees around the reflective surface
		// Trace from reflected light source to original collision point to get the "reflected" light

		Uint8 red = tempRed > 255 ? 255 : tempRed;
		Uint8 green = tempGreen > 255 ? 255 : tempGreen;
		Uint8 blue = tempBlue > 255 ? 255 : tempBlue;

		return RGB(red, green, blue);
	}

	Vector World::GetEyeRay(int x, int y, int windowWidth, int windowHeight)
	{
		int maxDim = windowWidth > windowHeight ? windowWidth : windowHeight;
		float s = (2.f * x - windowWidth) / maxDim;
		float t = (windowHeight - 2.f * y) / maxDim;

		Vector ray;
		ray.m_x = m_forward.m_x + s*m_right.m_x + t*m_up.m_x;
		ray.m_y = m_forward.m_y + s*m_right.m_y + t*m_up.m_y;
		ray.m_z = m_forward.m_z + s*m_right.m_z + t*m_up.m_z;

		ray.normalize();
		return ray;
	}

	template <typename T>
	bool World::GetClosestCollision(const Vector &rayOrigin, const std::vector<const T*> &objects, const void *originObject, const Vector &ray, Collision &closestCollision)
	{
		float closestCollisionDist = closestCollision.distance;
		bool foundCollision = false;

		float collisionDist;
		Vector cPoint;

		for (const T *obj : objects)
		{
			// See if there is a collision, that the collision distance is less than our current closest, and that the collision distance is greater than zero (with a little room for rounding errors).
			if (obj != originObject && obj->CheckCollision(rayOrigin, ray, collisionDist, cPoint) && collisionDist < closestCollisionDist && collisionDist > ZERO_WITH_MARGIN_OF_ERROR)
			{
				foundCollision = true;
				closestCollisionDist = collisionDist;
				closestCollision = Collision(obj, 
					cPoint, 
					collisionDist,
					obj->GetColorAt(cPoint),
					obj->GetNormalAt(cPoint, ray),
					obj->GetAlpha(),
					obj->GetReflectivity());
			}
		}

		return foundCollision;
	}

	template <typename T>
	void World::TraceRayFromCollisionToPointLight(const std::vector<const T*> &objects, const Collision &collision, const Vector &lightDirection, float distanceToLight, float &lightRayAlpha)
	{
		if (lightRayAlpha <= 0.f)
		{
			return;
		}

		float dist;
		Vector cp;
		bool hasCollision;

		const void *collisionObject = collision.object;
		const Vector& collisionPoint = collision.collisionPoint;
		float objectAlpha = collision.objectAlpha;

		for (const T *obj : objects)
		{
			if (obj != collisionObject)
			{
				hasCollision = obj->CheckCollision(collisionPoint, lightDirection, dist, cp);
				if (hasCollision && dist < distanceToLight)
				{
					lightRayAlpha -= objectAlpha;
					if (lightRayAlpha <= 0.f)
					{
						break;
					}
				}
			}
		}
	}

	template <typename T>
	void World::TraceRayFromCollisionToSun(const std::vector<const T*> &objects, const Collision &collision, const Vector &lightDirection, float &sunRayAlpha)
	{
		if (sunRayAlpha <= 0.f)
		{
			return;
		}

		float dist;
		Vector cp;
		bool hasCollision;

		const void *collisionObject = collision.object;
		const Vector& collisionPoint = collision.collisionPoint;
		float objectAlpha = collision.objectAlpha;

		for (const T *obj : objects)
		{
			if (obj != collisionObject)
			{
				hasCollision = obj->CheckCollision(collisionPoint, lightDirection, dist, cp);
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
	}

	inline bool World::SortByDistToEye(const Collision &c1, const Collision &c2)
	{
		return c1.distance < c2.distance;
	}

	void World::DrawWorldSubset(SDL_Surface *surface, int beginY, int endY)
	{
		int windowWidth = surface->w;
		int windowHeight = surface->h;
		int maxDim = windowWidth > windowHeight ? windowWidth : windowHeight;

		float s, t;
		Vector ray;

		for (int y = beginY; y < endY; ++y)
		{
			for (int x = 0; x < windowWidth; ++x)
			{
				s = (2.f * x - windowWidth) / maxDim;
				t = (windowHeight - 2.f * y) / maxDim;

				ray.m_x = m_forward.m_x + s*m_right.m_x + t*m_up.m_x;
				ray.m_y = m_forward.m_y + s*m_right.m_y + t*m_up.m_y;
				ray.m_z = m_forward.m_z + s*m_right.m_z + t*m_up.m_z;

				ray.normalize();

				COLORREF color = TraceRay(m_eye, ray, nullptr, 1.f, 0);
				SetSurfacePixel(surface, x, y, GetRValue(color), GetGValue(color), GetBValue(color));
			}
		}
	}

	inline void World::SetSurfacePixel(SDL_Surface *surface, int x, int y, Uint8 red, Uint8 green, Uint8 blue)
	{
		Uint8 *target = (Uint8 *)surface->pixels + (y * surface->pitch) + (x * Surface_Pixel_Size);
		*target = blue;
		*(target + 1) = green;
		*(target + 2) = red;
	}
}
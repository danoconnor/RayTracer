#include "stdafx.h"
#include "World.h"

namespace RayTracer
{
	World::World() : m_eye(0, 0, 0), m_forward(0, 0, -1), m_right(1, 0, 0), m_up(0, 1, 0)
	{
	}

	World::~World()
	{
		for (WorldObject *object : m_worldObjects)
		{
			delete object;
		}

		for (LightSource *sun : m_suns)
		{
			delete sun;
		}

		for (LightSource *pointLight : m_pointLights)
		{
			delete pointLight;
		}
	}

	void World::DrawWorld(SDL_Surface *surface)
	{
		int windowWidth = surface->w;
		int windowHeight = surface->h;
		int maxDim = windowWidth > windowHeight ? windowWidth : windowHeight;

		float s, t;
		Vector ray;

		for (int y = 0; y < windowHeight; ++y)
		{
			for (int x = 0; x < windowWidth; ++x)
			{
				s = (2.f * x - windowWidth) / maxDim;
				t = (windowHeight - 2.f * y) / maxDim;

				ray.m_x = m_forward.m_x + s*m_right.m_x + t*m_up.m_x;
				ray.m_y = m_forward.m_y + s*m_right.m_y + t*m_up.m_y;
				ray.m_z = m_forward.m_z + s*m_right.m_z + t*m_up.m_z;

				ray.normalize();

				COLORREF color = TraceRay(ray);
				SetSurfacePixel(surface, x, y, GetRValue(color), GetGValue(color), GetBValue(color));
			}
		}
	}

	void World::AddWorldObject(const WorldObject &object)
	{
		m_worldObjects.push_back(new WorldObject(object));
	}

	void World::AddSun(const LightSource &sun)
	{
		m_suns.push_back(new LightSource(sun));
	}

	void World::AddPointLight(const LightSource &light)
	{
		m_pointLights.push_back(new LightSource(light));
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

	COLORREF World::TraceRay(const Vector &ray)
	{
		std::vector<Collision> collisions;

		for (WorldObject *obj : m_worldObjects)
		{
			float collisionDist;
			Vector cPoint;
			if (obj->CheckCollision(m_eye, ray, collisionDist, cPoint))
			{
				collisions.push_back(Collision(obj, cPoint, collisionDist));
			}
		}

		std::sort(collisions.begin(), collisions.end(), &World::SortByDistToEye); // Sort so that the closest points are at the front of the list

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

				float distToBulb = xDiff / lightDir.m_x; // Calculate T-value, not true distance

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

						int r = (int)(floor(GetRValue(lightColor) * GetRValue(objectColor) * dot * lightRayAlpha / (Color_Divide_Constant))) + red;
						int g = (int)(floor(GetGValue(lightColor) * GetGValue(objectColor) * dot * lightRayAlpha / (Color_Divide_Constant))) + green;
						int b = (int)(floor(GetBValue(lightColor) * GetBValue(objectColor) * dot * lightRayAlpha / (Color_Divide_Constant))) + blue;

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

						int r = (int)(floor(GetRValue(lightColor) * GetRValue(objectColor) * dot * sunRayAlpha / (Color_Divide_Constant))) + red;
						int g = (int)(floor(GetGValue(lightColor) * GetGValue(objectColor) * dot * sunRayAlpha / (Color_Divide_Constant))) + green;
						int b = (int)(floor(GetBValue(lightColor) * GetBValue(objectColor) * dot * sunRayAlpha / (Color_Divide_Constant))) + blue;

						red = r > 255 ? 255 : r;
						green = g > 255 ? 255 : g;
						blue = b > 255 ? 255 : b;
					}
				}
			}

			int r = (int)(floor(totalRed + (red * collision.object->GetAlpha() * eyeRayAlpha)));
			int g = (int)(floor(totalGreen + (green * collision.object->GetAlpha() * eyeRayAlpha)));
			int b = (int)(floor(totalBlue + (blue * collision.object->GetAlpha() * eyeRayAlpha)));

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

		return COLORREF();
	}

	inline bool World::SortByDistToEye(const Collision &c1, const Collision &c2)
	{
		return c1.distance < c2.distance;
	}

	inline void World::SetSurfacePixel(SDL_Surface *surface, int x, int y, Uint8 red, Uint8 green, Uint8 blue)
	{
		Uint8 *target = (Uint8 *)surface->pixels + y * surface->pitch + x * Surface_Pixel_Size;
		*target = blue;
		*(target + 1) = green;
		*(target + 2) = red;
	}
}
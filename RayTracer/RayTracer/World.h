#pragma once
#include "stdafx.h"

#include "WorldObject.h"
#include "LightSource.h"

namespace RayTracer
{
	class World
	{
		public:
			World();
			~World();

			void DrawWorld(SDL_Surface *surface);

			void AddWorldObject(const WorldObject &object);
			void AddSun(const LightSource &sun);
			void AddPointLight(const LightSource &light);

			void StepForward(float amount);
			void StepBackward(float amount);
			void StepRight(float amount);
			void StepLeft(float amount);
			
			void LookUp(float angle);
			void LookDown(float angle);
			void LookLeft(float angle);
			void LookRight(float angle);

			void MoveUp(float amount);
			void MoveDown(float amount);

			void SetEye(const Vector &newEye);
			const Vector& GetEye(); // To change the value of the eye vector, use SetEye

			void SetForward(const Vector &newForward);
			const Vector& GetForward(); // To change the value of the forward vector, use SetForward

			const Vector& GetUp();
			const Vector& GetRight();

		private:
			struct Collision
			{
				Collision(WorldObject *obj, const Vector &point, float dist)
				{
					object = obj;
					collisionPoint = point;
					distance = dist;
				}

				WorldObject *object;
				Vector collisionPoint;
				float distance; // distance from collisionPoint to the origin of the ray
			};

			static inline bool SortByDistToEye(const Collision &c1, const Collision &c2);

			COLORREF TraceRay(const Vector &ray);

			inline void SetSurfacePixel(SDL_Surface *surface, int x, int y, Uint8 red, Uint8 green, Uint8 blue);

			Vector m_eye;
			Vector m_forward;
			Vector m_right;
			Vector m_up;

			std::vector<WorldObject *> m_worldObjects;
			std::vector<LightSource *> m_suns;
			std::vector<LightSource *> m_pointLights;

			std::vector<cimg_library::CImg<unsigned char>> m_loadedTexures;

			static const short Surface_Pitch = 2560;
			static const short Surface_Pixel_Size = 4;
			static const Uint8 Color_Divide_Constant = 255;
	};
}

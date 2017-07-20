#pragma once
#include "stdafx.h"

#include "LightSource.h"
#include "TrianglePlane.h"
#include "Sphere.h"
#include "RectangularPlane.h"

namespace RayTracer
{
	class World
	{
		public:
			World();
			~World();

			void DrawWorld(SDL_Surface *surface);

			void AddTriangle(const TrianglePlane *object);
			void AddSphere(const Sphere *object);
			void AddRectangle(const RectangularPlane *object);
			void AddSun(const LightSource *sun);
			void AddPointLight(const LightSource *light);

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
			const Vector& GetEye();

			void SetForward(const Vector &newForward);
			const Vector& GetForward();

			const Vector& GetUp();
			const Vector& GetRight();

			void SetAmbientLight(float ambientLightAlpha);

			// Trace ray starting at the origin point and going in the ray direction. 
			// originObject is used in reflection as the object that the ray originates from. It will be null for rays originating at the eye point.
			// eyeRayAlpha is used when the main ray casted from the eye is passing though multiple translucent objects.
			// reflectionRecursion is to make sure that we don't get stuck in an infinite loop of reflections. It will start at 0 for rays coming from the eye and gets incremented everytime the ray is reflected.
			COLORREF TraceRay(const Vector &rayOrigin, const Vector &ray, const void *originObject, float eyerayAlpha, Uint8 reflectionRecursion);

			// Only used for debugging. It's more performant to leave duplicate logic between here and DrawWorldSubset
			// Takes a window position (x, y) and calculates the ray that corresponds to that pixel
			Vector GetEyeRay(int x, int y, int windowWidth, int windowHeight);

		private:
			struct Collision
			{
				Collision() {}

				Collision(const void *obj, const Vector &point, float dist, COLORREF color, const Vector &normal, float alpha, float reflectivity)
				{
					object = obj;
					collisionPoint = point;
					distance = dist;
					objectColor = color;
					objectNormal = normal;
					objectAlpha = alpha;
					objectReflectivity = reflectivity;
				}

				// Reference to the object that was collided with. This is used during lighting to make sure that an object doesn't cast a shadow on itself.
				const void *object;
				
				// The point of the collision
				Vector collisionPoint;
				
				// Distance from collisionPoint to the origin of the ray
				float distance;

				// The object's color at the collision point
				COLORREF objectColor;

				// The normal of the object at the collision point.
				Vector objectNormal; 

				// The alpha of the object that was collided with.
				float objectAlpha;

				// The reflectivity of the object that was collided with
				float objectReflectivity;
			};

			struct ReflectedLight
			{
				ReflectedLight(const void *obj, const Vector& reflectionPoint, const std::vector<const LightSource*> &pointLights, const std::vector<const LightSource*> &suns)
				{
					reflectingObject = obj;
					objectReflectionPoint = reflectionPoint;
					reflectedPointLights = pointLights;
					reflectedSuns = suns;
				}

				// The object that is reflecting the light
				const void *reflectingObject;

				// The point on the reflecting object that is considered to reflect the light. Generally the center of the object (for simplicity and optimization).
				Vector objectReflectionPoint;

				// Point lights that are in their reflected positions
				std::vector<const LightSource*> reflectedPointLights;

				// Suns that are in their reflected directions
				std::vector<const LightSource*> reflectedSuns;
			};

			static inline bool SortByDistToEye(const Collision &c1, const Collision &c2);

			// Checks for any collisions.
			// If a collision is found, the function returns true and populates the closestCollision parameter.
			template <typename T> 
			bool GetClosestCollision(const Vector &rayOrigin, const std::vector<const T*> &objects, const void *originObject, const Vector &ray, Collision &closestCollision);

			// Traces a ray from the collision point to a given light source (point light or sun). It updates the lightRayAlpha parameter as it encounters any object between the collision and light source.
			template <typename T>
			void TraceRayFromCollisionToLight(const std::vector<const T*> &objects, const Collision &collision, const Vector &lightDirection, float distanceToLight, const void *reflectingObject, float &lightRayAlpha);

			void DrawWorldSubset(SDL_Surface *surface, int beginY, int endY);
			inline void SetSurfacePixel(SDL_Surface *surface, int x, int y, Uint8 red, Uint8 green, Uint8 blue);

			// Recalculates the locations of all the reflected lights in the world. Should be called whenever an object or light source is added/removed.
			void RefreshReflectedLights();

			Vector m_eye;
			Vector m_forward;
			Vector m_right;
			Vector m_up;

			float m_ambientAlpha;

			std::vector<const TrianglePlane *> m_triangles;
			std::vector<const Sphere *> m_spheres;
			std::vector<const RectangularPlane *> m_rectangles;

			std::vector<const LightSource *> m_suns;
			std::vector<const LightSource *> m_pointLights;
			std::vector<ReflectedLight> m_reflectedLights;

			std::vector<cimg_library::CImg<unsigned char>> m_loadedTexures;

			const Uint8 Color_Divide_Constant = 255;
			const short Surface_Pixel_Size = 4;
	};
}

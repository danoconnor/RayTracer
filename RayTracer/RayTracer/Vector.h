#pragma once

namespace RayTracer
{
	class Vector
	{
		public:
			inline static Vector Cross(const Vector &v1, const Vector &v2) { return Vector(v1.m_y*v2.m_z - v1.m_z*v2.m_y, v1.m_z*v2.m_x - v1.m_x*v2.m_z, v1.m_x*v2.m_y - v1.m_y*v2.m_x); }
			inline static float Dot(const Vector &v1, const Vector &v2) { return v1.m_x*v2.m_x + v1.m_y*v2.m_y + v1.m_z*v2.m_z; }

			float m_x;
			float m_y;
			float m_z;

			Vector();
			Vector(float x, float y, float z);

			float dot(const Vector &other) const;

			void scalarMult(float scalar);
			void normalize();

			Vector operator+(const Vector &other) const;
			Vector operator-(const Vector &other) const;
			bool operator==(const Vector &other) const;
			bool operator!=(const Vector &other) const;
	};
}
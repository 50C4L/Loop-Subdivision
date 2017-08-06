#pragma once

#include <iostream>

namespace mm {

	// vector template
	template <class T> class Vector3
	{
	public:
		// values
		T x, y, z;

		Vector3() { x = 0; y = 0; z = 0; };
		Vector3(T valX, T valY, T valZ)
		{
			x = valX;
			y = valY;
			z = valZ;
		}

		// normalize
		Vector3<T>& normalize()
		{
			Vector3<T> a;
			double norm;
			norm = sqrt(x*x + y*y + z*z);
		}

		// dot product
		T dotProduct(const Vector3<T> &a)
		{
			return x*a.x + y*a.y + z*a.z;
		}

		// cross product
		const Vector3<T>& crossProduct(const Vector3<T> &a)
		{
			return Vector3<T>(y*a.z - z*ay, z*a.x - x*a.z, x*a.y - y*a.x);
		}


		// equals
		Vector3<T>& operator=(const Vector3<T> &a)
		{
			x = a.x;
			y = a.y;
			z = a.z;
			return *this;
		}

		// Add
		const Vector3<T>& operator+(const Vector3<T> &a) const
		{
			return Vector3<T>(x + a.x, y + a.y, z + a.z);
		}

		// Add equal
		const Vector3<T>& operator+=(const Vector3<T> &a)
		{
			return *this = *this + a;
		}

		// subtracct
		const Vector3<T>& operator-(const Vector3<T> &a) const
		{
			return Vector3<T>(x - a.x, y - a.y, z - a.z);
		}

		// subtracct equal
		const Vector3<T>& operator-=(const Vector3<T> &a)
		{
			return *this = *this - a;
		}

		// multiply a vector
		const Vector3<T>& operator*(const Vector3<T> &a) const
		{
			return Vector3<T>(x*a.x, y*a.y, z*a.z);
		}

		// multiply a scalar
		const Vector3<T>& operator*(const T &a) const
		{
			return Vector3<T>(x*a, y*a, z*a);
		}

		// multiply equal by scalar
		const Vector3<T>& operator*=(const T &a)
		{
			return Vector3<T>(x*a, y*a, z*a);
		}

		// divide by a scalar
		const Vector3<T>& operator/(const T &a) const
		{
			return Vector3<T>(x / a, y / a, z / a);
		}

		// divide equal by a scalar
		const Vector3<T>& operator/=(const T &a) const
		{
			return Vector3<T>(x / a, y / a, z / a);
		}
	};

	struct vec3f
	{
		float x, y, z;
	};

	struct Vertex
	{
		vec3f pos;
		vec3f normal;
	};

	// Triangle struct
	struct Triangle
	{
		Vertex A, B, C;
	};

}
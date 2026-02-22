#pragma once

#include <immintrin.h>
#include <iostream>
#include <cmath>
#include "vect4f.hpp"

class alignas(16) mat4f
{
	public:
		vect4f       cols[4];

		/* Constructors */
		inline mat4f()
		{
			cols[0] = vect4f(1.0f, 0.0f, 0.0f, 0.0f);
			cols[1] = vect4f(0.0f, 1.0f, 0.0f, 0.0f);
			cols[2] = vect4f(0.0f, 0.0f, 1.0f, 0.0f);
			cols[3] = vect4f(0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline mat4f(mat4f const &) = default;

		inline mat4f(vect4f const& a, vect4f const& b, vect4f const& c, vect4f const& d)
		{
			cols[0] = a;
			cols[1] = b;
			cols[2] = c;
			cols[3] = d;
		}

		/* Destructor */
		inline ~mat4f() = default;

		/* Operators */
		inline mat4f & operator=(mat4f const &) = default;

		inline mat4f operator+(mat4f const & rhs) const
		{
			return mat4f(
				cols[0] + rhs.cols[0],
				cols[1] + rhs.cols[1],
				cols[2] + rhs.cols[2],
				cols[3] + rhs.cols[3]
			);
		}

		inline mat4f operator-(mat4f const & rhs) const
		{
			return mat4f(
				cols[0] - rhs.cols[0],
				cols[1] - rhs.cols[1],
				cols[2] - rhs.cols[2],
				cols[3] - rhs.cols[3]
			);
		}

		inline vect4f operator*(vect4f const & v) const
		{
			__m128 e0 = _mm_set1_ps(v.getX());
			__m128 e1 = _mm_set1_ps(v.getY());
			__m128 e2 = _mm_set1_ps(v.getZ());
			__m128 e3 = _mm_set1_ps(v.getW());

			__m128 r = _mm_add_ps(
				_mm_add_ps(_mm_mul_ps(cols[0].getMM(), e0), _mm_mul_ps(cols[1].getMM(), e1)),
				_mm_add_ps(_mm_mul_ps(cols[2].getMM(), e2), _mm_mul_ps(cols[3].getMM(), e3))
			);

			return vect4f(r);
		}

		inline mat4f operator*(mat4f const & rhs) const
		{
			return mat4f(
				(*this) * rhs.cols[0],
				(*this) * rhs.cols[1],
				(*this) * rhs.cols[2],
				(*this) * rhs.cols[3]
			);
		}

		/* Accessors */
		inline vect4f & operator[](int i) { return cols[i]; }
		inline const vect4f & operator[](int i) const { return cols[i]; }

		/* Static Functions */
		static inline mat4f identity()
		{
			return mat4f();
		}

		static inline mat4f translation(vect4f const& t)
		{
			return mat4f(
				vect4f(1.0f, 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, 1.0f, 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, 1.0f, 0.0f),
				vect4f(t.getX(), t.getY(), t.getZ(), 1.0f) // Getters
			);
		}

		static inline mat4f rotation(float angle, vect4f const& axis)
		{
			vect4f u = axis.normalized();

			float c = std::cos(angle);
			float s = std::sin(angle);
			float t = 1.0f - c;

			// Getters pour extraire les scalaires
			float x = u.getX();
			float y = u.getY();
			float z = u.getZ();

			return mat4f(
				vect4f(t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0.0f),
				vect4f(t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0.0f),
				vect4f(t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0.0f),
				vect4f(0.0f, 0.0f, 0.0f, 1.0f)
			);
		}

		static inline mat4f scale(vect4f const& s)
		{
			return mat4f(
				vect4f(s.getX(), 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, s.getY(), 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, s.getZ(), 0.0f),
				vect4f(0.0f, 0.0f, 0.0f, 1.0f)
			);
		}

		static inline mat4f perspective(float fov, float aspect, float near, float far)
		{
			float f = 1.0f / std::tan(fov * 0.5f);
			float nf = 1.0f / (near - far);

			return mat4f(
				vect4f(f / aspect, 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, f, 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, (far + near) * nf, -1.0f),
				vect4f(0.0f, 0.0f, (2.0f * far * near) * nf, 0.0f)
			);
		}

		static inline mat4f lookAt(vect4f const & eye, vect4f const & center, vect4f const & up)
		{
			vect4f f = (center - eye).normalized();
			vect4f s = f.cross(up).normalized();
			vect4f u = s.cross(f);

			return mat4f(
				vect4f(s.getX(), u.getX(), -f.getX(), 0.0f),
				vect4f(s.getY(), u.getY(), -f.getY(), 0.0f),
				vect4f(s.getZ(), u.getZ(), -f.getZ(), 0.0f),
				vect4f(-s.dot(eye), -u.dot(eye), f.dot(eye), 1.0f)
			);
		}
};

/* Stream insertion operator */
inline std::ostream& operator<<(std::ostream& os, mat4f const & m)
{
	os << "\n";
	for (int row = 0; row < 4; ++row)
	{
		os << "| "
		<< m.cols[0][row] << " "
		<< m.cols[1][row] << " "
		<< m.cols[2][row] << " "
		<< m.cols[3][row] << " |\n";
	}
	return os;
}

inline mat4f operator*(float s, mat4f const & m)
{
	return mat4f(
		m.cols[0] * s,
		m.cols[1] * s,
		m.cols[2] * s,
		m.cols[3] * s
	);
}
#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.
constexpr Mat44f operator*(const Mat44f& aLeft, const Mat44f& aRight) noexcept {
	Mat44f result{};

	for (std::size_t i = 0; i < 4; ++i) {
		for (std::size_t j = 0; j < 4; ++j) {
			result(i, j) = 0.0f;
			for (std::size_t k = 0; k < 4; ++k) {
				result(i, j) += aLeft(i, k) * aRight(k, j);
			}
		}
	}

	return result;
}

constexpr Vec4f operator*(const Mat44f& aLeft, const Vec4f& aRight) noexcept {
	Vec4f result{};

	for (std::size_t i = 0; i < 4; ++i) {
		result[i] = 0.0f;
		for (std::size_t j = 0; j < 4; ++j) {
			result[i] += aLeft(i, j) * aRight[j];
		}
	}

	return result;
}
// Functions:
inline
Mat44f transpose(Mat44f const& aM) noexcept
{
	Mat44f ret;
	for (std::size_t i = 0; i < 4; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
			ret(j, i) = aM(i, j);
	}
	return ret;
}

inline
Mat44f make_rotation_x(float aAngle) noexcept
{
	float cosA = std::cos(aAngle);
	float sinA = std::sin(aAngle);
	return {
		1.f, 0.f, 0.f, 0.f,
		0.f, cosA, -sinA, 0.f,
		0.f, sinA, cosA, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

}


inline
Mat44f make_rotation_y(float aAngle) noexcept
{
	float cosA = std::cos(aAngle);
	float sinA = std::sin(aAngle);
	return {
		cosA, 0.f, sinA, 0.f,
		0.f, 1.f, 0.f, 0.f,
		-sinA, 0.f, cosA, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_rotation_z(float aAngle) noexcept
{
	float cosA = std::cos(aAngle);
	float sinA = std::sin(aAngle);
	return {
		cosA, -sinA, 0.f, 0.f,
		sinA, cosA, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_translation(Vec3f aTranslation) noexcept
{
	return {
		1.f, 0.f, 0.f, aTranslation[0],
		0.f, 1.f, 0.f, aTranslation[1],
		0.f, 0.f, 1.f, aTranslation[2],
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_perspective_projection(float aFovInRadians, float aAspect, float aNear, float aFar) noexcept
{
	float tanHalfFov = tan(aFovInRadians / 2.0f);

	// Calculate matrix elements
	float s = 1.0f / tanHalfFov;
	float nf = 1.0f / (aFar - aNear);

	Mat44f result{};
	result(0, 0) = s / aAspect;
	result(1, 1) = 1.0f / tanHalfFov;
	result(2, 2) = -(aFar + aNear) * nf;
	result(2, 3) = -2.0f * aFar * aNear * nf;
	result(3, 2) = -1.0f;

	return result;
}


inline
Mat44f make_scaling(float aSX, float aSY, float aSZ) noexcept {
	Mat44f scalingMatrix{};
	scalingMatrix(0, 0) = aSX;
	scalingMatrix(1, 1) = aSY;
	scalingMatrix(2, 2) = aSZ;
	scalingMatrix(3, 3) = 1.0f; // Homogeneous coordinate

	return scalingMatrix;
}


#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

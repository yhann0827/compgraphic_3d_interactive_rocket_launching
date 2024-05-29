#ifndef VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD
#define VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD

#include <cmath>
#include <cassert>
#include <cstdlib>

struct Vec3f
{
	float x, y, z;

	constexpr 
	float& operator[] (std::size_t aI) noexcept
	{
		assert( aI < 3 );
		return aI[&x]; // This is a bit sketchy.
	}
	constexpr 
	float operator[] (std::size_t aI) const noexcept
	{
		assert( aI < 3 );
		return aI[&x]; // This is a bit sketchy.
	}
};


constexpr
Vec3f operator+( Vec3f aVec ) noexcept
{
	return aVec;
}
constexpr
Vec3f operator-( Vec3f aVec ) noexcept
{
	return { -aVec.x, -aVec.y, -aVec.z };
}

constexpr
Vec3f operator+( Vec3f aLeft, Vec3f aRight ) noexcept
{
	return Vec3f{
		aLeft.x + aRight.x,
		aLeft.y + aRight.y,
		aLeft.z + aRight.z
	};
}
constexpr
Vec3f operator-( Vec3f aLeft, Vec3f aRight ) noexcept
{
	return Vec3f{
		aLeft.x - aRight.x,
		aLeft.y - aRight.y,
		aLeft.z - aRight.z
	};
}


constexpr
Vec3f operator*( float aScalar, Vec3f aVec ) noexcept
{
	return Vec3f{ 
		aScalar * aVec.x, 
		aScalar * aVec.y, 
		aScalar * aVec.z
	};
}
constexpr
Vec3f operator*( Vec3f aVec, float aScalar ) noexcept
{
	return aScalar * aVec;
}

constexpr
Vec3f operator/( Vec3f aVec, float aScalar ) noexcept
{
	return Vec3f{ 
		aVec.x / aScalar,
		aVec.y / aScalar,
		aVec.z / aScalar
	};
}


constexpr
Vec3f& operator+=( Vec3f& aLeft, Vec3f aRight ) noexcept
{
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft;
}
constexpr
Vec3f& operator-=( Vec3f& aLeft, Vec3f aRight ) noexcept
{
	aLeft.x -= aRight.x;
	aLeft.y -= aRight.y;
	aLeft.z -= aRight.z;
	return aLeft;
}

constexpr
Vec3f& operator*=( Vec3f& aLeft, float aRight ) noexcept
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	aLeft.z *= aRight;
	return aLeft;
}
constexpr
Vec3f& operator/=( Vec3f& aLeft, float aRight ) noexcept
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	aLeft.z /= aRight;
	return aLeft;
}


// A few common functions:

constexpr
float dot( Vec3f aLeft, Vec3f aRight ) noexcept
{
	return aLeft.x * aRight.x 
		+ aLeft.y * aRight.y
		+ aLeft.z * aRight.z
	;
}

inline
float length( Vec3f aVec ) noexcept
{
	// The standard function std::sqrt() is not marked as constexpr. length()
	// calls std::sqrt() unconditionally, so length() cannot be marked
	// constexpr itself.
	return std::sqrt( dot( aVec, aVec ) );
}

inline
Vec3f normalize( Vec3f aVec ) noexcept
{
	auto const l = length( aVec );
	return aVec / l;
}

inline
Vec3f cross(const Vec3f& a, const Vec3f& b) noexcept
{
    return Vec3f{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline
Vec3f quadraticBezier(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, float t) {
    float u = 1.0f - t;
    Vec3f result = (u * u) * p0 + 2.0f * u * t * p1 + (t * t) * p2;

    return result;
}

inline
Vec3f quadraticBezierTangent(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, float t) {
    float u = 1.0f - t;

    Vec3f tangent = 2.0f * (u * (p1 - p0) + t * (p2 - p1));

    return tangent;
}

#endif // VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD

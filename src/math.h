#pragma once

#include <algorithm>
#include <limits>


namespace Ushuaia
{

struct Matrix4x4;
struct Matrix3x3;


template <typename T>
constexpr T Clamp(T v, T _min, T _max)
{
	return std::min(std::max(_min, v), _max);
}


template <typename T>
struct TVec2
{
	union {
		T v[2];

		struct {
			T x, y;
		};
	};

	constexpr T& operator[](int i);
	constexpr T operator[](int i) const;

	constexpr void Set(T _v0, T _v1);

	constexpr bool operator==(TVec2 const & rhs) const;

	constexpr TVec2 operator+(TVec2 const & rhs) const;
	constexpr TVec2 operator-(TVec2 const & rhs) const;
	constexpr TVec2 operator*(T s) const;
	constexpr TVec2 operator/(T s) const;

	constexpr void operator+=(TVec2 const & rhs);
	constexpr void operator-=(TVec2 const & rhs);
	constexpr void operator*=(T s);
	constexpr void operator/=(T s);

	constexpr T Dot(TVec2 const & rhs) const;
	inline T Length() const;
	inline T Normalize();
};

typedef TVec2<float>	Vector2;


template <typename T>
struct TVec3
{
	union {
		T v[3];

		struct {
			T x, y, z;
		};
	};

	constexpr T& operator[](int i);
	constexpr T operator[](int i) const;

	constexpr void Set(T _v0, T _v1, T _v2);

	constexpr bool operator==(TVec3 const & rhs) const;

	inline TVec3 operator+(TVec3 const & rhs) const;
	inline TVec3 operator-(TVec3 const & rhs) const;
	inline TVec3 operator*(T s) const;
	inline TVec3 operator/(T s) const;

	inline void operator+=(TVec3 const & rhs);
	inline void operator-=(TVec3 const & rhs);
	inline void operator*=(T s);
	inline void operator/=(T s);

	inline void TransformBy(Matrix3x3 const & m);
	inline void TransformBy(Matrix4x4 const & m);

	constexpr T Dot(TVec3 const & rhs) const;
	constexpr T Length() const;
	constexpr T Normalize();
};

typedef TVec3<float>	Vector3;


template <typename T>
struct TVec4
{
	union {
		T v[4];

		struct {
			T x, y, z, w;
		};
	};

	constexpr T& operator[](int i);
	constexpr T operator[](int i) const;

	constexpr TVec3<T> & Vec3();

	constexpr void Set(T _v0, T _v1, T _v2, T _v3);

	constexpr bool operator==(TVec4 const & rhs) const;

	inline TVec4 operator*(T r) const;
	inline TVec4 operator/(T r) const;

	inline void operator*=(T r);
	inline void operator/=(T r);

	constexpr void TransformBy(Matrix4x4 const & m);
};

typedef TVec4<float>	Vector4;


struct Matrix4x4
{
	float v[16];

	constexpr float& operator[](int i);
	constexpr float operator[](int i) const;

	inline void SetIdentity();
	inline void SetSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t);

	inline void Transform(Vector4 & out, Vector4 const & in) const;
	inline void TransformVec3(Vector4 & out, Vector4 const & in) const;
	inline void Transform(Vector3 & out, Vector3 const & in) const;

	constexpr void Mtx4x3(float * out) const;
};


struct Matrix3x3
{
	float v[9];

	constexpr float& operator[](int i);
	constexpr float operator[](int i) const;

	inline void SetIdentity();

	constexpr void Transform(Vector3 & out, Vector3 const & in) const;
};


template <typename T>
constexpr bool Overlap1D(T min1, T max1, T min2, T max2) {
	return max1 > min2 && max2 > min1;
}
template <typename T>
constexpr bool Contain1D(T min1, T max1, T min2, T max2) {
	return min1 <= min2 && max1 >= max2;
}


template <typename T>
struct TRect
{
	typedef TVec2<T>	DataType;

	DataType rMin;
	DataType rMax;

	constexpr void Set(DataType const & _min, DataType const & _size);

	constexpr T Width() const;
	constexpr T Height() const;

	constexpr DataType Size() const;
	constexpr DataType Center() const;

	constexpr bool IsEmpty() const;

	constexpr void Union(TRect<T> const & b);

	constexpr void Scale(float s);

	constexpr bool operator==(TRect<T> const & rhs) const;

	constexpr bool Overlap(TRect<T> const & rhs) const;

	constexpr bool Conatain(TRect<T> const & rhs) const;
};


struct Box
{
	Vector3 bMin;
	Vector3 bMax;

	constexpr void Set(Vector3 const & _min, Vector3 const & _size);

	constexpr float Width() const;
	constexpr float Height() const;
	constexpr float Depth() const;

	constexpr Vector3 Size() const;
	inline Vector3 Center() const;

	constexpr bool IsEmpty() const;

	constexpr void Union(Box const & b);

	inline void Scale(float s);

	constexpr bool operator==(Box const & rhs) const;

	constexpr bool Overlap(Box const & rhs) const;

	constexpr bool Conatain(Box const & rhs) const;
};

}

#include "math.inl"

#pragma once

#include <algorithm>
#include <limits>


namespace Ushuaia
{

struct Matrix4x4;
struct Matrix3x3;


template <typename T>
constexpr T clamp(T v, T _min, T _max)
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

	constexpr void set(T _v0, T _v1);

	constexpr bool operator==(TVec2 const & rhs) const;

	constexpr TVec2 operator+(TVec2 const & rhs) const;
	constexpr TVec2 operator-(TVec2 const & rhs) const;
	constexpr TVec2 operator*(T s) const;
	constexpr TVec2 operator/(T s) const;

	constexpr void operator+=(TVec2 const & rhs);
	constexpr void operator-=(TVec2 const & rhs);
	constexpr void operator*=(T s);
	constexpr void operator/=(T s);

	constexpr T dot(TVec2 const & rhs) const;
	inline T length() const;
	inline T normalize();
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

	constexpr void set(T _v0, T _v1, T _v2);

	constexpr bool operator==(TVec3 const & rhs) const;

	inline TVec3 operator+(TVec3 const & rhs) const;
	inline TVec3 operator-(TVec3 const & rhs) const;
	inline TVec3 operator*(T s) const;
	inline TVec3 operator/(T s) const;

	inline void operator+=(TVec3 const & rhs);
	inline void operator-=(TVec3 const & rhs);
	inline void operator*=(T s);
	inline void operator/=(T s);

	inline void transformBy(Matrix3x3 const & m);
	inline void transformBy(Matrix4x4 const & m);

	constexpr T dot(TVec3 const & rhs) const;
	constexpr T length() const;
	constexpr T normalize();
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

	constexpr TVec3<T> & vec3();

	constexpr void set(T _v0, T _v1, T _v2, T _v3);

	constexpr bool operator==(TVec4 const & rhs) const;

	inline TVec4 operator*(T r) const;
	inline TVec4 operator/(T r) const;

	inline void operator*=(T r);
	inline void operator/=(T r);

	constexpr void transformBy(Matrix4x4 const & m);
};

typedef TVec4<float>	Vector4;


struct Matrix4x4
{
	float v[16];

	constexpr float& operator[](int i);
	constexpr float operator[](int i) const;

	inline void setIdentity();
	inline void setSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t);

	inline void transform(Vector4 & out, Vector4 const & in) const;
	inline void transformVec3(Vector4 & out, Vector4 const & in) const;
	inline void transform(Vector3 & out, Vector3 const & in) const;

	constexpr void mtx4x3(float * out) const;
};


struct Matrix3x3
{
	float v[9];

	constexpr float& operator[](int i);
	constexpr float operator[](int i) const;

	inline void setIdentity();

	constexpr void transform(Vector3 & out, Vector3 const & in) const;
};


template <typename T>
constexpr bool overlap1D(T min1, T max1, T min2, T max2) {
	return max1 > min2 && max2 > min1;
}
template <typename T>
constexpr bool contain1D(T min1, T max1, T min2, T max2) {
	return min1 <= min2 && max1 >= max2;
}


template <typename T>
struct TRect
{
	typedef TVec2<T>	DataType;

	DataType rMin;
	DataType rMax;

	constexpr void set(DataType const & _min, DataType const & _size);

	constexpr T width() const;
	constexpr T height() const;

	constexpr DataType size() const;
	constexpr DataType center() const;

	constexpr bool isEmpty() const;

	constexpr void unionWith(TRect<T> const & b);

	constexpr void scale(float s);

	constexpr bool operator==(TRect<T> const & rhs) const;

	constexpr bool overlap(TRect<T> const & rhs) const;

	constexpr bool conatain(TRect<T> const & rhs) const;
};


struct Box
{
	Vector3 bMin;
	Vector3 bMax;

	constexpr void set(Vector3 const & _min, Vector3 const & _size);

	constexpr float width() const;
	constexpr float height() const;
	constexpr float depth() const;

	constexpr Vector3 size() const;
	inline Vector3 center() const;

	constexpr bool isEmpty() const;

	constexpr void unionWith(Box const & b);

	inline void scale(float s);

	constexpr bool operator==(Box const & rhs) const;

	constexpr bool overlap(Box const & rhs) const;

	constexpr bool conatain(Box const & rhs) const;
};

}

#include "math.inl"

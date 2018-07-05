#pragma once

#include <algorithm>
#include <limits>


namespace Ushuaia
{

struct Matrix4x4;
struct Matrix3x3;


template <typename T>
inline T clamp(T v, T _min, T _max)
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

	inline T& operator[](int i);
	inline T operator[](int i) const;

	inline void set(T _v0, T _v1);

	inline bool operator==(TVec2 const & rhs) const;

	inline TVec2 operator+(TVec2 const & rhs) const;
	inline TVec2 operator-(TVec2 const & rhs) const;
	inline TVec2 operator*(T s) const;
	inline TVec2 operator/(T s) const;

	inline void operator+=(TVec2 const & rhs);
	inline void operator-=(TVec2 const & rhs);
	inline void operator*=(T s);
	inline void operator/=(T s);

	inline T dot(TVec2 const & rhs) const;
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

	inline T& operator[](int i);
	inline T operator[](int i) const;

	inline void set(T _v0, T _v1, T _v2);

	inline bool operator==(TVec3 const & rhs) const;

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

	inline T dot(TVec3 const & rhs) const;
	inline T length() const;
	inline T normalize();
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

	inline T& operator[](int i);
	inline T operator[](int i) const;

	inline TVec3<T> & vec3();

	inline void set(T _v0, T _v1, T _v2, T _v3);

	inline bool operator==(TVec4 const & rhs) const;

	inline TVec4 operator*(T r) const;
	inline TVec4 operator/(T r) const;

	inline void operator*=(T r);
	inline void operator/=(T r);

	inline void transformBy(Matrix4x4 const & m);
};

typedef TVec4<float>	Vector4;


struct Matrix4x4
{
	float v[16];

	inline float& operator[](int i);
	inline float operator[](int i) const;

	inline void setIdentity();
	inline void setSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t);

	inline void transform(Vector4 & out, Vector4 const & in) const;
	inline void transformVec3(Vector4 & out, Vector4 const & in) const;
	inline void transform(Vector3 & out, Vector3 const & in) const;

	inline void mtx4x3(float * out) const;
};


struct Matrix3x3
{
	float v[9];

	inline float& operator[](int i);
	inline float operator[](int i) const;

	inline void setIdentity();

	inline void transform(Vector3 & out, Vector3 const & in) const;
};


template <typename T>
inline bool overlap1D(T min1, T max1, T min2, T max2) {
	return max1 > min2 && max2 > min1;
}
template <typename T>
inline bool contain1D(T min1, T max1, T min2, T max2) {
	return min1 <= min2 && max1 >= max2;
}


template <typename T>
struct TRect
{
	typedef TVec2<T>	DataType;

	DataType rMin;
	DataType rMax;

	inline void set(DataType const & _min, DataType const & _size);

	inline T width() const;
	inline T height() const;

	inline DataType size() const;
	inline DataType center() const;

	inline bool isEmpty() const;

	inline void unionWith(TRect<T> const & b);

	inline void scale(float s);

	inline bool operator==(TRect<T> const & rhs) const;

	inline bool overlap(TRect<T> const & rhs) const;

	inline bool conatain(TRect<T> const & rhs) const;
};


struct Box
{
	Vector3 bMin;
	Vector3 bMax;

	inline void set(Vector3 const & _min, Vector3 const & _size);

	inline float width() const;
	inline float height() const;
	inline float depth() const;

	inline Vector3 size() const;
	inline Vector3 center() const;

	inline bool isEmpty() const;

	inline void unionWith(Box const & b);

	inline void scale(float s);

	inline bool operator==(Box const & rhs) const;

	inline bool overlap(Box const & rhs) const;

	inline bool conatain(Box const & rhs) const;
};

}

#include "math.inl"

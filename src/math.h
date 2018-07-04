#pragma once

#include <algorithm>
#include <limits>
#include <cassert>
#include <bx/math.h>


namespace My3D
{

struct Matrix4x4;
struct Matrix3x3;
struct Matrix4x3;


template <typename T>
T clamp(T v, T _min, T _max)
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

	inline T& operator[](int i) {
		return v[i];
	}
	inline T const & operator[](int i) const {
		return v[i];
	}

	inline void Set(T _v0, T _v1) {
		v[0] = _v0; v[1] = _v1;
	}

	inline bool operator==(TVec2 const & rhs) const {
		return v[0] == rhs.v[0] && v[1] == rhs.v[1];
	}

	inline TVec2 operator+(TVec2 const & rhs) const {
		TVec2 ret;
		ret.Set(v[0] + rhs.v[0], v[1] + rhs.v[1]);
		return std::move(ret);
	}

	inline TVec2 operator-(TVec2 const & rhs) const {
		TVec2 ret;
		ret.Set(v[0] - rhs.v[0], v[1] - rhs.v[1]);
		return std::move(ret);
	}

	inline TVec2 operator*(T m) const {
		TVec2 ret;
		ret.Set(v[0] * m, v[1] * m);
		return std::move(ret);
	}

	inline TVec2 operator/(T m) const {
		TVec2 ret{ v[0] / m, v[1] / m };
		return std::move(ret);
	}

	inline void operator+=(TVec2 const & rhs) {
		Set(v[0] + rhs.v[0], v[1] + rhs.v[1]);
	}

	inline void operator-=(TVec2 const & rhs) {
		Set(v[0] - rhs.v[0], v[1] - rhs.v[1]);
	}

	inline void operator*=(T r) {
		Set(v[0] * r, v[1] * r);
	}

	inline void operator/=(T r) {
		Set(v[0] / r, v[1] / r);
	}

	inline T dot(TVec2 const & rhs) const {
		T ret = x * rhs.x + y * rhs.y;
		return ret;
	}

	inline void transformBy(T const * pM) {
		x = x * pM[0] + y * pM[2];
		y = x * pM[1] + y * pM[3];
	}

	inline T length() const {
		T sqr = dot(*this);
		if (sqr < 1e-6f)
			return 0;
		return bx::sqrt(sqr);
	}
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

	inline T& operator[](int i) {
		return v[i];
	}
	inline T const & operator[](int i) const {
		return v[i];
	}

	//TVec3& operator=(TVec3 const & rhs) {
	//	v[0] = rhs.v[0];
	//	v[1] = rhs.v[1];
	//	v[2] = rhs.v[2];
	//	return *this;
	//}

	inline void Set(T _v0, T _v1, T _v2) {
		v[0] = _v0; v[1] = _v1; v[2] = _v2;
	}

	inline bool operator==(TVec3 const & rhs) const {
		return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2];
	}

	inline TVec3 operator+(TVec3 const & rhs) const {
		TVec3 ret;
		bx::vec3Add(ret.v, v, rhs.v);
		return std::move(ret);
	}

	inline TVec3 operator-(TVec3 const & rhs) const {
		TVec3 ret;
		bx::vec3Sub(ret.v, v, rhs.v);
		return std::move(ret);
	}

	inline TVec3 operator*(T m) const {
		TVec3 ret;
		bx::vec3Mul(ret.v, v, m);
		return std::move(ret);
	}

	inline TVec3 operator/(T d) const {
		TVec3 ret;
		bx::vec3Mul(ret.v, v, 1.f/d);
		return std::move(ret);
	}

	inline void operator+=(TVec3 const & rhs) {
		bx::vec3Add(v, v, rhs.v);
	}

	inline void operator-=(TVec3 const & rhs) {
		bx::vec3Sub(v, v, rhs.v);
	}

	inline void operator*=(T m) {
		bx::vec3Mul(v, v, r);
	}

	inline void operator/=(T d) {
		bx::vec3Mul(v, v, 1.f/r);
	}

	void transformBy(Matrix3x3 const & mtx) {
		x = x * pM[0] + y * pM[3] + z * pM[6] + pM[9];
		y = x * pM[1] + y * pM[4] + z * pM[7] + pM[10];
		z = x * pM[2] + y * pM[5] + z * pM[8] + pM[11];
	}

	inline T dot(TVec3 const & rhs) const {
		T ret = x * rhs.x + y * rhs.y + z * rhs.z;
		return ret;
	}

	inline T length() const {
		T sqr = dot(*this);
		if (sqr < 1e-6f)
			return 0;
		return sqrtf(sqr);
	}

	inline TVec3 & normalize() {
		T len = length();
		if (len > 1e-6f) {
			T inv = 1.f / len;
			*this *= inv;
		}
		return *this;
	}
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

	inline T& operator[](int i) {
		return v[i];
	}
	inline T const & operator[](int i) const {
		return v[i];
	}

	inline void Set(T _v0, T _v1, T _v2, T _v3) {
		v[0] = _v0; v[1] = _v1; v[2] = _v2; v[3] = _v3;
	}

	inline bool operator==(TVec4 const & rhs) const {
		return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2] && v[3] == rhs.v[3];
	}

	inline TVec4 operator+(TVec4 const & rhs) const {
		TVec4 ret;
		ret.Set(v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]);
		return std::move(ret);
	}

	inline TVec4 operator-(TVec4 const & rhs) const {
		TVec4 ret;
		ret.Set(v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]);
		return std::move(ret);
	}

	inline TVec4 operator*(T m) const {
		TVec4 ret;
		ret.Set(v[0] * m, v[1] * m, v[2] * m, v[3] * m);
		return std::move(ret);
	}

	inline TVec4 operator/(T m) const {
		TVec4 ret;
		ret.Set(v[0] / m, v[1] / m, v[2] / m, v[3] / m);
		return std::move(ret);
	}

	inline void operator+=(TVec4 const & rhs) {
		Set(v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]);
	}

	inline void operator-=(TVec4 const & rhs) {
		Set(v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]);
	}

	inline void operator*=(T r) {
		Set(v[0] * r, v[1] * r, v[2] * r, v[3] * r);
	}

	inline void operator/=(T r) {
		Set(v[0] / r, v[1] / r, v[2] / r, v[3] / r);
	}

	void transformBy(T const * pM) {
		x = x * pM[0] + y * pM[4] + z * pM[8] + pM[12];
		y = x * pM[1] + y * pM[5] + z * pM[9] + pM[13];
		z = x * pM[2] + y * pM[6] + z * pM[10] + pM[14];
		w = x * pM[3] + y * pM[7] + z * pM[11] + pM[15];
	}

	inline T dot(TVec4 const & rhs) const {
		T ret = x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		return ret;
	}

	inline T length() const {
		T sqr = dot(*this);
		if (sqr < 1e-6f)
			return 0;
		return sqrtf(sqr);
	}

	inline TVec4 & normalize() {
		T len = length();
		if (len > 1e-6f) {
			T inv = 1.f / len;
			*this *= inv;
		}
		return *this;
	}
};

typedef TVec4<float>	Vector4;


struct Matrix4x4
{
	float v[16];

	inline float& operator[](int i) {
		return v[i];
	}
	inline float const & operator[](int i) const {
		return v[i];
	}

	void setIdentity()
	{
		memset(v, 0, sizeof(v));
		v[0] = v[5] = v[10] = v[15] = 1.0f;
	}
};


template <typename T>
bool overlap1D(T min1, T max1, T min2, T max2) {
	return max1 > min2 && max2 > min1;
}
template <typename T>
bool contain1D(T min1, T max1, T min2, T max2) {
	return min1 <= min2 && max1 >= max2;
}


template <typename T>
struct TRect
{
	typedef TVec2<T>	DataType;

	DataType rMin;
	DataType rMax;

	inline void Set(DataType const & _min, DataType const & _size) {
		rMin = _min;
		rMax.Set(_min.x + _size.x, _min.y + _size.y);
	}

	inline T Width() const {
		return rMax.x - rMin.x;
	}
	inline T Height() const {
		return rMax.y - rMin.y;
	}

	inline DataType Size() const {
		DataType ret;
		ret.Set( Width(), Height() );
		return std::move(ret);
	}
	inline DataType Center() const {
		assert(!IsEmpty());
		return (rMin + rMax) / 2;
	}

	inline bool IsEmpty() const {
		return (rMin.x > rMax.x) || (rMin.y > rMax.y);
	}

	inline void Union(TRect<T> const & b) {
		rMin.Set(std::min(rMin.x, b.rMin.x), std::min(rMin.y, b.rMin.y));
		rMax.Set(std::max(rMax.x, b.rMax.x), std::max(rMax.y, b.rMax.y));
	}

	inline void Scale(float s) {
		DataType const & sw = Size() * (s * 0.5f);
		rMin -= sw;
		rMax += sw;
	}

	inline bool operator==(TRect<T> const & rhs) const {
		return rMin == rhs.rMin && rMax == rhs.rMax;
	}

	inline bool Overlap(TRect<T> const & rhs) const {
		return !IsEmpty()
			&& overlap1D(rMin.x, rMax.x, rhs.rMin.x, rhs.rMax.x)
			&& overlap1D(rMin.y, rMax.y, rhs.rMin.y, rhs.rMax.y);
	}

	inline bool Conatain(TRect<T> const & rhs) const {
		return !IsEmpty() && !rhs.IsEmpty()
			&& contain1D(rMin.x, rMax.x, rhs.rMin.x, rhs.rMax.x)
			&& contain1D(rMin.y, rMax.y, rhs.rMin.y, rhs.rMax.y);
	}
};


struct Box
{
	Vector3 bMin;
	Vector3 bMax;

	void Set(Vector3 const & _min, Vector3 const & _size) {
		bMin = _min;
		bMax.Set(_min.x + _size.x, _min.y + _size.y, _min.z + _size.z);
	}

	inline float Width() const {
		return bMax.x - bMin.x;
	}
	inline float Height() const {
		return bMax.y - bMin.y;
	}
	inline float Depth() const {
		return bMax.z - bMin.z;
	}

	inline Vector3 Size() const {
		Vector3 ret;
		ret.Set(Width(), Height(), Depth());
		return std::move(ret);
	}
	inline Vector3 Center() const {
		assert(!IsEmpty());
		return (bMin + bMax) / 2;
	}

	inline bool IsEmpty() const {
		return (bMin.x > bMax.x) || (bMin.y > bMax.y) || (bMin.z > bMax.z);
	}

	inline void Union(Box const & b) {
		bMin.Set(std::min(bMin.x, b.bMin.x), std::min(bMin.y, b.bMin.y), std::min(bMin.z, b.bMin.z));
		bMax.Set(std::max(bMax.x, b.bMax.x), std::max(bMax.y, b.bMax.y), std::max(bMax.z, b.bMax.z));
	}

	inline void Scale(float s) {
		auto const & sw = Size() * (s * 0.5f);
		bMin -= sw;
		bMax += sw;
	}

	inline bool operator==(Box const & rhs) const {
		return bMin == rhs.bMin && bMax == rhs.bMax;
	}

	inline bool Overlap(Box const & rhs) const {
		return !IsEmpty()
			&& overlap1D(bMin.x, bMax.x, rhs.bMin.x, rhs.bMax.x)
			&& overlap1D(bMin.y, bMax.y, rhs.bMin.y, rhs.bMax.y)
			&& overlap1D(bMin.z, bMax.z, rhs.bMin.z, rhs.bMax.z);
	}


	inline bool Conatain(Box const & rhs) const {
		return !IsEmpty() && !rhs.IsEmpty()
			&& contain1D(bMin.x, bMax.x, rhs.bMin.x, rhs.bMax.x)
			&& contain1D(bMin.y, bMax.y, rhs.bMin.y, rhs.bMax.y)
			&& contain1D(bMin.z, bMax.z, rhs.bMin.z, rhs.bMax.z);
	}
};

}

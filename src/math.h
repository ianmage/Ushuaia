#pragma once

#include <algorithm>
#include <limits>
#include <cassert>
#include <bx/math.h>


namespace Ushuaia
{

struct Matrix4x4;
struct Matrix3x3;


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

	inline T length() const {
		return bx::sqrt(dot(*this));
	}

	inline T normalize() {
		float const len = length();
		float const invLen = 1.0f / len;
		x = x * invLen;
		y = y * invLen;
		return len;
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

	void transformBy(Matrix3x3 const & m) {
		T nX = x * m[0] + y * m[3] + z * m[6];
		T nY = x * m[1] + y * m[4] + z * m[7];
		T nZ = x * m[2] + y * m[5] + z * m[8];
		Set(nX, nY, nZ);
	}

	void transformBy(Matrix4x4 const & m) {
		TVec3<T> result;
		bx::vec3MulMtx(result.v, v, m.v);
		*this = result;
	}

	inline T dot(TVec3 const & rhs) const {
		return bx::vec3Dot(v, rhs.v);
	}

	inline T length() const {
		return bx::vec3Length(v);
	}

	inline T normalize() {
		return bx::vec3Norm(v, v);
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

	TVec3<T> & vec3() {
		return *reinterpret_cast<TVec3<T>*>(v);
	}

	inline void Set(T _v0, T _v1, T _v2, T _v3) {
		v[0] = _v0; v[1] = _v1; v[2] = _v2; v[3] = _v3;
	}

	inline bool operator==(TVec4 const & rhs) const {
		return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2] && v[3] == rhs.v[3];
	}

	inline TVec4 operator*(T r) const {
		TVec4 ret;
		bx::vec4Mul(ret.v, v, r.v);
		return std::move(ret);
	}

	inline TVec4 operator/(T r) const {
		TVec4 ret;
		bx::vec4Mul(ret.v, v, 1.f/r.v);
		return std::move(ret);
	}

	inline void operator*=(T r) {
		bx::vec4Mul(v, v, r.v);
	}

	inline void operator/=(T r) {
		bx::vec4Mul(v, v, 1.f/r.v);
	}

	void transformBy(Matrix4x4 const & m) {
		TVec4<T> result;
		bx::vec4MulMtx(result.v, v, m.v);
		*this = result;
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

	void setIdentity() {
		memset(v, 0, sizeof(v));
		v[0] = v[5] = v[10] = v[15] = 1.0f;
	}

	void setSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t) {
		bx::mtxSRT(v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
	}

	void transform(Vector4 & out, Vector4 const & in) const {
		bx::vec4MulMtx(out.v, in.v, v);
	}

	void transformVec3(Vector4 & out, Vector4 const & in) const {
		bx::vec3MulMtx(out.v, in.v, v);
		out[4] = in[4];
	}

	void transform(Vector3 & out, Vector3 const & in) const {
		bx::vec3MulMtx(out.v, in.v, v);
	}

	void mtx4x3(float * out) const {
		int i = 0;
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				out[i++] = v[j * 4 + k];
			}
		}
	}
};


struct Matrix3x3
{
	float v[9];

	inline float& operator[](int i) {
		return v[i];
	}
	inline float const & operator[](int i) const {
		return v[i];
	}

	void setIdentity()
	{
		memset(v, 0, sizeof(v));
		v[0] = v[4] = v[8] = 1.0f;
	}

	void transform(Vector3 & out, Vector3 const & in) const {
		out.x = in.x * v[0] + in.y * v[3] + in.z * v[6];
		out.y = in.x * v[1] + in.y * v[4] + in.z * v[7];
		out.z = in.x * v[2] + in.y * v[5] + in.z * v[8];
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

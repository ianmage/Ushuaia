#pragma once

#include <limits>
#include <cassert>
#include <bx/math.h>


namespace Ushuaia
{

template <typename T>
inline T& TVec2<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
inline T TVec2<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
inline void TVec2<T>::set(T _v0, T _v1)
{
	v[0] = _v0; v[1] = _v1;
}

template <typename T>
inline bool TVec2<T>::operator==(TVec2 const & rhs) const
{
	return v[0] == rhs.v[0] && v[1] == rhs.v[1];
}

template <typename T>
inline TVec2<T> TVec2<T>::operator+(TVec2 const & rhs) const
{
	TVec2<T> ret;
	ret.set(v[0] + rhs.v[0], v[1] + rhs.v[1]);
	return std::move(ret);
}

template <typename T>
inline TVec2<T> TVec2<T>::operator-(TVec2 const & rhs) const
{
	TVec2 ret;
	ret.set(v[0] - rhs.v[0], v[1] - rhs.v[1]);
	return std::move(ret);
}

template <typename T>
inline TVec2<T> TVec2<T>::operator*(T s) const
{
	TVec2 ret;
	ret.set(v[0] * s, v[1] * s);
	return std::move(ret);
}

template <typename T>
inline TVec2<T> TVec2<T>::operator/(T s) const
{
	TVec2<T> ret{ v[0] / s, v[1] / s };
	return std::move(ret);
}

template <typename T>
inline void TVec2<T>::operator+=(TVec2 const & rhs)
{
	set(v[0] + rhs.v[0], v[1] + rhs.v[1]);
}

template <typename T>
inline void TVec2<T>::operator-=(TVec2 const & rhs)
{
	set(v[0] - rhs.v[0], v[1] - rhs.v[1]);
}

template <typename T>
inline void TVec2<T>::operator*=(T s)
{
	set(v[0] * s, v[1] * s);
}

template <typename T>
inline void TVec2<T>::operator/=(T s)
{
	set(v[0] / s, v[1] / s);
}

template <typename T>
inline T TVec2<T>::dot(TVec2 const & rhs) const
{
	T ret = x * rhs.x + y * rhs.y;
	return ret;
}

template <typename T>
inline T TVec2<T>::length() const
{
	return bx::sqrt(dot(*this));
}

template <typename T>
inline T TVec2<T>::normalize()
{
	float const len = length();
	float const invLen = 1.0f / len;
	x = x * invLen;
	y = y * invLen;
	return len;
}


template <typename T>
inline T& TVec3<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
inline T TVec3<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
inline void TVec3<T>::set(T _v0, T _v1, T _v2)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2;
}

template <typename T>
inline bool TVec3<T>::operator==(TVec3 const & rhs) const
{
	return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2];
}

template <typename T>
inline TVec3<T> TVec3<T>::operator+(TVec3 const & rhs) const
{
	TVec3<T> ret;
	bx::vec3Add(ret.v, v, rhs.v);
	return std::move(ret);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator-(TVec3 const & rhs) const
{
	TVec3<T> ret;
	bx::vec3Sub(ret.v, v, rhs.v);
	return std::move(ret);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator*(T s) const
{
	TVec3<T> ret;
	bx::vec3Mul(ret.v, v, s);
	return std::move(ret);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator/(T s) const
{
	TVec3<T> ret;
	bx::vec3Mul(ret.v, v, 1.f/s);
	return std::move(ret);
}

template <typename T>
inline void TVec3<T>::operator+=(TVec3 const & rhs)
{
	bx::vec3Add(v, v, rhs.v);
}

template <typename T>
inline void TVec3<T>::operator-=(TVec3 const & rhs)
{
	bx::vec3Sub(v, v, rhs.v);
}

template <typename T>
inline void TVec3<T>::operator*=(T m)
{
	bx::vec3Mul(v, v, m);
}

template <typename T>
inline void TVec3<T>::operator/=(T d)
{
	bx::vec3Mul(v, v, 1.f/d);
}

template <typename T>
void TVec3<T>::transformBy(Matrix3x3 const & m)
{
	T nX = x * m[0] + y * m[3] + z * m[6];
	T nY = x * m[1] + y * m[4] + z * m[7];
	T nZ = x * m[2] + y * m[5] + z * m[8];
	set(nX, nY, nZ);
}

template <typename T>
void TVec3<T>::transformBy(Matrix4x4 const & m)
{
	TVec3<T> result;
	bx::vec3MulMtx(result.v, v, m.v);
	*this = result;
}

template <typename T>
inline T TVec3<T>::dot(TVec3 const & rhs) const
{
	return bx::vec3Dot(v, rhs.v);
}

template <typename T>
inline T TVec3<T>::length() const
{
	return bx::vec3Length(v);
}

template <typename T>
inline T TVec3<T>::normalize()
{
	return bx::vec3Norm(v, v);
}


template <typename T>
inline T& TVec4<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
inline T TVec4<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
inline TVec3<T> & TVec4<T>::vec3()
{
	return *reinterpret_cast<TVec3<T>*>(v);
}

template <typename T>
inline void TVec4<T>::set(T _v0, T _v1, T _v2, T _v3)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2; v[3] = _v3;
}

template <typename T>
inline bool TVec4<T>::operator==(TVec4 const & rhs) const
{
	return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2] && v[3] == rhs.v[3];
}

template <typename T>
inline TVec4<T> TVec4<T>::operator*(T s) const
{
	TVec4<T> ret;
	bx::vec4Mul(ret.v, v, s);
	return std::move(ret);
}

template <typename T>
inline TVec4<T> TVec4<T>::operator/(T s) const
{
	TVec4<T> ret;
	bx::vec4Mul(ret.v, v, 1.f/s);
	return std::move(ret);
}

template <typename T>
inline void TVec4<T>::operator*=(T s)
{
	bx::vec4Mul(v, v, s);
}

template <typename T>
inline void TVec4<T>::operator/=(T s)
{
	bx::vec4Mul(v, v, 1.f/s);
}

template <typename T>
void TVec4<T>::transformBy(Matrix4x4 const & m)
{
	TVec4<T> result;
	bx::vec4MulMtx(result.v, v, m.v);
	*this = result;
}


inline float& Matrix4x4::operator[](int i)
{
	return v[i];
}
inline float Matrix4x4::operator[](int i) const
{
	return v[i];
}

void Matrix4x4::setIdentity()
{
	memset(v, 0, sizeof(v));
	v[0] = v[5] = v[10] = v[15] = 1.0f;
}

void Matrix4x4::setSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t)
{
	bx::mtxSRT(v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
}

void Matrix4x4::transform(Vector4 & out, Vector4 const & in) const
{
	bx::vec4MulMtx(out.v, in.v, v);
}

void Matrix4x4::transformVec3(Vector4 & out, Vector4 const & in) const
{
	bx::vec3MulMtx(out.v, in.v, v);
	out[4] = in[4];
}

void Matrix4x4::transform(Vector3 & out, Vector3 const & in) const
{
	bx::vec3MulMtx(out.v, in.v, v);
}

void Matrix4x4::mtx4x3(float * out) const
{
	int i = 0;
	for (int j = 0; j < 4; ++j)
	{
		for (int k = 0; k < 3; ++k)
		{
			out[i++] = v[j * 4 + k];
		}
	}
}


inline float& Matrix3x3::operator[](int i)
{
	return v[i];
}
inline float Matrix3x3::operator[](int i) const
{
	return v[i];
}

void Matrix3x3::setIdentity()
{
	memset(v, 0, sizeof(v));
	v[0] = v[4] = v[8] = 1.0f;
}

void Matrix3x3::transform(Vector3 & out, Vector3 const & in) const
{
	out.x = in.x * v[0] + in.y * v[3] + in.z * v[6];
	out.y = in.x * v[1] + in.y * v[4] + in.z * v[7];
	out.z = in.x * v[2] + in.y * v[5] + in.z * v[8];
}


template <typename T>
inline void TRect<T>::set(TRect<T>::DataType const & _min, TRect<T>::DataType const & _size)
{
	rMin = _min;
	rMax.set(_min.x + _size.x, _min.y + _size.y);
}

template <typename T>
inline T TRect<T>::width() const
{
	return rMax.x - rMin.x;
}
template <typename T>
inline T TRect<T>::height() const
{
	return rMax.y - rMin.y;
}

template <typename T>
inline typename TRect<T>::DataType TRect<T>::size() const
{
	DataType ret;
	ret.Set( width(), height() );
	return std::move(ret);
}
template <typename T>
inline typename TRect<T>::DataType TRect<T>::center() const
{
	assert(!isEmpty());
	return (rMin + rMax) / 2;
}

template <typename T>
inline bool TRect<T>::isEmpty() const
{
	return (rMin.x > rMax.x) || (rMin.y > rMax.y);
}

template <typename T>
inline void TRect<T>::unionWith(TRect<T> const & b)
{
	rMin.Set(std::min(rMin.x, b.rMin.x), std::min(rMin.y, b.rMin.y));
	rMax.Set(std::max(rMax.x, b.rMax.x), std::max(rMax.y, b.rMax.y));
}

template <typename T>
inline void TRect<T>::scale(float s)
{
	DataType const & sw = size() * (s * 0.5f);
	rMin -= sw;
	rMax += sw;
}

template <typename T>
inline bool TRect<T>::operator==(TRect<T> const & rhs) const
{
	return rMin == rhs.rMin && rMax == rhs.rMax;
}

template <typename T>
inline bool TRect<T>::overlap(TRect<T> const & rhs) const
{
	return !isEmpty()
		&& overlap1D(rMin.x, rMax.x, rhs.rMin.x, rhs.rMax.x)
		&& overlap1D(rMin.y, rMax.y, rhs.rMin.y, rhs.rMax.y);
}

template <typename T>
inline bool TRect<T>::conatain(TRect<T> const & rhs) const
{
	return !isEmpty() && !rhs.isEmpty()
		&& contain1D(rMin.x, rMax.x, rhs.rMin.x, rhs.rMax.x)
		&& contain1D(rMin.y, rMax.y, rhs.rMin.y, rhs.rMax.y);
}


void Box::set(Vector3 const & _min, Vector3 const & _size)
{
	bMin = _min;
	bMax.set(_min.x + _size.x, _min.y + _size.y, _min.z + _size.z);
}

inline float Box::width() const
{
	return bMax.x - bMin.x;
}
inline float Box::height() const
{
	return bMax.y - bMin.y;
}
inline float Box::depth() const
{
	return bMax.z - bMin.z;
}

inline Vector3 Box::size() const
{
	Vector3 ret;
	ret.set(width(), height(), depth());
	return std::move(ret);
}
inline Vector3 Box::center() const
{
	assert(!isEmpty());
	return (bMin + bMax) / 2;
}

inline bool Box::isEmpty() const
{
	return (bMin.x > bMax.x) || (bMin.y > bMax.y) || (bMin.z > bMax.z);
}

inline void Box::unionWith(Box const & b)
{
	bMin.set(std::min(bMin.x, b.bMin.x), std::min(bMin.y, b.bMin.y), std::min(bMin.z, b.bMin.z));
	bMax.set(std::max(bMax.x, b.bMax.x), std::max(bMax.y, b.bMax.y), std::max(bMax.z, b.bMax.z));
}

inline void Box::scale(float s)
{
	auto const & sw = size() * (s * 0.5f);
	bMin -= sw;
	bMax += sw;
}

inline bool Box::operator==(Box const & rhs) const
{
	return bMin == rhs.bMin && bMax == rhs.bMax;
}

inline bool Box::overlap(Box const & rhs) const
{
	return !isEmpty()
		&& overlap1D(bMin.x, bMax.x, rhs.bMin.x, rhs.bMax.x)
		&& overlap1D(bMin.y, bMax.y, rhs.bMin.y, rhs.bMax.y)
		&& overlap1D(bMin.z, bMax.z, rhs.bMin.z, rhs.bMax.z);
}


inline bool Box::conatain(Box const & rhs) const
{
	return !isEmpty() && !rhs.isEmpty()
		&& contain1D(bMin.x, bMax.x, rhs.bMin.x, rhs.bMax.x)
		&& contain1D(bMin.y, bMax.y, rhs.bMin.y, rhs.bMax.y)
		&& contain1D(bMin.z, bMax.z, rhs.bMin.z, rhs.bMax.z);
}

}

#include <cmath>
#include <limits>
#include <cassert>
#include <bx/math.h>


namespace Ushuaia
{

template <typename T>
constexpr T& TVec2<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
constexpr T TVec2<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
constexpr void TVec2<T>::Set(T _v0, T _v1)
{
	v[0] = _v0; v[1] = _v1;
}

template <typename T>
constexpr bool TVec2<T>::operator==(TVec2 const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1];
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator+(TVec2 const & other) const
{
	return TVec2(x+other.x, y+other.y);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator-(TVec2 const & other) const
{
	return TVec2(x-other.x, y-other.y);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator*(TVec2 const & other) const
{
	return TVec2(x*other.x, y*other.y);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator*(T s) const
{
	return TVec2(x*s, y*s);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator/(T s) const
{
	return TVec2(v[0] / s, v[1] / s);
}

template <typename T>
constexpr void TVec2<T>::operator+=(TVec2 const & other)
{
	Set(v[0] + other.v[0], v[1] + other.v[1]);
}

template <typename T>
constexpr void TVec2<T>::operator-=(TVec2 const & other)
{
	Set(v[0] - other.v[0], v[1] - other.v[1]);
}

template <typename T>
constexpr void TVec2<T>::operator*=(TVec2 const & other)
{
	Set(v[0] * other.v[0], v[1] * other.v[1]);
}

template <typename T>
constexpr void TVec2<T>::operator*=(T s)
{
	Set(v[0] * s, v[1] * s);
}

template <typename T>
constexpr void TVec2<T>::operator/=(T s)
{
	Set(v[0] / s, v[1] / s);
}

template <typename T>
constexpr T TVec2<T>::Dot(TVec2 const & other) const
{
	T ret = x * other.x + y * other.y;
	return ret;
}

template <typename T>
inline T TVec2<T>::Length() const
{
	return ::sqrtf(Dot(*this));
}

template <typename T>
inline T TVec2<T>::Normalize()
{
	T const len = Length();
	T const invLen = 1.0f / len;
	x = x * invLen;
	y = y * invLen;
	return len;
}


template <typename T>
constexpr T& TVec3<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
constexpr T TVec3<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
constexpr void TVec3<T>::Set(T _v0, T _v1, T _v2)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2;
}

template <typename T>
constexpr bool TVec3<T>::operator==(TVec3 const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2];
}

template <typename T>
constexpr TVec3<T> TVec3<T>::operator+(TVec3 const & other) const
{
	return TVec3(x+other.x, y+other.y, z+other.z);
}

template <typename T>
constexpr TVec3<T> TVec3<T>::operator-(TVec3 const & other) const
{
	return TVec3(x-other.x, y-other.y, z-other.z);
}

template <typename T>
constexpr TVec3<T> TVec3<T>::operator*(TVec3 const & other) const
{
	return TVec3(x*other.x, y*other.y, z*other.z);
}

template <typename T>
constexpr TVec3<T> TVec3<T>::operator*(T s) const
{
	return TVec3(x*s, y*s, z*s);
}

template <typename T>
constexpr TVec3<T> TVec3<T>::operator/(T s) const
{
	s = 1.f / s;
	return TVec3(x*s, y*s, z*s);
}

template <typename T>
constexpr void TVec3<T>::operator+=(TVec3 const & other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

template <typename T>
constexpr void TVec3<T>::operator-=(TVec3 const & other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

template <typename T>
constexpr void TVec3<T>::operator*=(TVec3 const & other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
}

template <typename T>
constexpr void TVec3<T>::operator*=(T m)
{
	x *= m;
	y *= m;
	z *= m;
}

template <typename T>
constexpr void TVec3<T>::operator/=(T d)
{
	T m = 1.f / d;
	x *= m;
	y *= m;
	z *= m;
}

template <typename T>
constexpr void TVec3<T>::TransformBy(Matrix3x3 const & m)
{
	T nX = x * m[0] + y * m[3] + z * m[6];
	T nY = x * m[1] + y * m[4] + z * m[7];
	T nZ = x * m[2] + y * m[5] + z * m[8];
	Set(nX, nY, nZ);
}

template <typename T>
constexpr void TVec3<T>::TransformBy(Matrix4x4 const & m)
{
	T nX = x * m[0] + y * m[4] + z * m[ 8] + m[12];
	T nY = x * m[1] + y * m[5] + z * m[ 9] + m[13];
	T nZ = x * m[2] + y * m[6] + z * m[10] + m[14];
	Set(nX, nY, nZ);
}

template <typename T>
constexpr T TVec3<T>::Dot(TVec3 const & other) const
{
	return x * other.x + y * other.y + z * other.z;
}

template <typename T>
constexpr T TVec3<T>::Length() const
{
	return ::sqrtf(Dot(*this));
}

template <typename T>
constexpr T TVec3<T>::Normalize()
{
	T len = Length();
	if (len < 1e-5f)
		return 0;
	operator/=(len);
	return len;
}

template <typename T>
constexpr TVec3<T> TVec3<T>::Cross(TVec3 const & other) const
{
	TVec3<T> ret;
	ret.x = y * other.z - other.y * z;
	ret.y = other.x * z - x * other.z;
	ret.z = x * other.y - other.x * y;
	return ret;
}


template <typename T>
constexpr TVec3<T> const & Mul(TVec3<T> & out, TVec3<T> const & v, Matrix3x3 const & m)
{
	out.x = v.x * m[0] + v.y * m[3] + v.z * m[6];
	out.y = v.x * m[1] + v.y * m[4] + v.z * m[7];
	out.z = v.x * m[2] + v.y * m[5] + v.z * m[8];
	return out;
}

template <typename T>
constexpr TVec3<T> const & Mul(TVec3<T> & out, TVec3<T> const & v, Matrix4x4 const & m)
{
	out.x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + m[12];
	out.y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + m[13];
	out.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
	return out;
}


template <typename T>
constexpr TVec3<T> CalcNormal(TVec3<T> const & _va, TVec3<T> const & _vb, TVec3<T> const & _vc)
{
	TVec3<T> ba = _vb - _va;
	TVec3<T> ca = _vc - _va;
	TVec3<T> baXca = ba.Cross(ca);
	baXca.Normalize();
	return baXca;
}


template <typename T>
constexpr T& TVec4<T>::operator[](int i)
{
	return v[i];
}
template <typename T>
constexpr T TVec4<T>::operator[](int i) const
{
	return v[i];
}

template <typename T>
constexpr TVec3<T> & TVec4<T>::Vec3()
{
	return *(TVec3<T>*)(v);
}

template <typename T>
constexpr TVec3<T> const & TVec4<T>::Vec3() const
{
	return *(TVec3<T> const *)(v);
}

template <typename T>
constexpr void TVec4<T>::Set(T _v0, T _v1, T _v2, T _v3)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2; v[3] = _v3;
}

template <typename T>
constexpr bool TVec4<T>::operator==(TVec4 const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2] && v[3] == other.v[3];
}

template <typename T>
constexpr TVec4<T> TVec4<T>::operator*(T s) const
{
	return TVec4(x*s, y*s, z*s, w*s);
}

template <typename T>
constexpr TVec4<T> TVec4<T>::operator/(T s) const
{
	s = 1.0f / s;
	return TVec4(x*s, y*s, z*s, w*s);
}

template <typename T>
constexpr void TVec4<T>::operator*=(T s)
{
	x *= s; y *= s;
	z *= s; w *= s;
}

template <typename T>
constexpr void TVec4<T>::operator/=(T s)
{
	s = 1.0f / s;
	x *= s; y *= s;
	z *= s; w *= s;
}

template <typename T>
constexpr void TVec4<T>::TransformBy(Matrix4x4 const & m)
{
	T nX = x * m[0] + y * m[4] + z * m[ 8] + m[12];
	T nY = x * m[1] + y * m[5] + z * m[ 9] + m[13];
	T nZ = x * m[2] + y * m[6] + z * m[10] + m[14];
	T nW = x * m[3] + y * m[7] + z * m[12] + m[15];
	Set(nX, nY, nZ, nW);
}


template <typename T>
constexpr TVec4<T> const & Mul(TVec4<T> & out, TVec4<T> const & v, Matrix4x4 const & m)
{
	out.x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + m[12];
	out.y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + m[13];
	out.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
	out.w = v.x * m[3] + v.y * m[7] + v.z * m[12] + m[15];
	return out;
}


constexpr float& Matrix4x4::operator[](int i)
{
	return v[i];
}
constexpr float Matrix4x4::operator[](int i) const
{
	return v[i];
}

constexpr void Matrix4x4::SetIdentity()
{
	v[0] = 1.0f;
	v[1] = v[2] = v[3] = v[4] = 0.0f;
	v[5] = 1.0f;
	v[6] = v[7] = v[8] = v[9] = 0.0f;
	v[10] = 1.0f;
	v[11] = v[12] = v[13] = v[14] = 0.0f;
	v[15] = 1.0f;
}

inline void Matrix4x4::SetSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t)
{
	bx::mtxSRT(v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
}

constexpr void Matrix4x4::Transform(Vector4 & out, Vector4 const & in) const
{
	out.x = in.x * v[0] + in.y * v[4] + in.z * v[ 8] + in.w * v[12];
	out.y = in.x * v[1] + in.y * v[5] + in.z * v[ 9] + in.w * v[13];
	out.z = in.x * v[2] + in.y * v[6] + in.z * v[10] + in.w * v[14];
	out.w = in.x * v[3] + in.y * v[7] + in.z * v[11] + in.w * v[15];
}

constexpr void Matrix4x4::TransformPos(Vector3 & out, Vector3 const & in) const
{
	out.x = in.x * v[0] + in.y * v[4] + in.z * v[ 8] + v[12];
	out.y = in.x * v[1] + in.y * v[5] + in.z * v[ 9] + v[13];
	out.z = in.x * v[2] + in.y * v[6] + in.z * v[10] + v[14];
}

constexpr void Matrix4x4::TransformDir(Vector3 & out, Vector3 const & in) const
{
	out.x = in.x * v[0] + in.y * v[4] + in.z * v[ 8];
	out.y = in.x * v[1] + in.y * v[5] + in.z * v[ 9];
	out.z = in.x * v[2] + in.y * v[6] + in.z * v[10];
}

constexpr void Matrix4x4::TransformPos(Vector4 & out, Vector4 const & in) const
{
	out.x = in.x * v[0] + in.y * v[4] + in.z * v[ 8] + v[12];
	out.y = in.x * v[1] + in.y * v[5] + in.z * v[ 9] + v[13];
	out.z = in.x * v[2] + in.y * v[6] + in.z * v[10] + v[14];
	out.w = in.w;
}

constexpr void Matrix4x4::TransformDir(Vector4 & out, Vector4 const & in) const
{
	out.x = in.x * v[0] + in.y * v[4] + in.z * v[ 8];
	out.y = in.x * v[1] + in.y * v[5] + in.z * v[ 9];
	out.z = in.x * v[2] + in.y * v[6] + in.z * v[10];
	out.w = in.w;
}

constexpr void Matrix4x4::Mtx4x3(float * out) const
{
	int i = 0;
	for (int j = 0; j < 4; ++j) {
		for (int k = 0; k < 3; ++k) {
			out[i++] = v[j * 4 + k];
		}
	}
}

inline void Matrix4x4::Transpose()
{
	for (int i = 0; i < 3; ++i) {
		for (int j = i+1; j < 4; ++j) {
			std::swap(v[i*4+j], v[j*4+i]);
		}
	}
}

inline Matrix4x4 Matrix4x4::GetInverse() const
{
	Matrix4x4 ret;
	bx::mtxInverse(ret.v, v);
	return ret;
}

constexpr Vector4 const & Matrix4x4::GetRow(uint8_t r) const
{
	return *(Vector4 const *)(&v[r * 4]);
}

void MtxMultiply(Matrix4x4 & out, Matrix4x4 const & lhs, Matrix4x4 const & rhs)
{
	bx::mtxMul(out.v, lhs.v, rhs.v);
}


constexpr float& Matrix3x3::operator[](int i)
{
	return v[i];
}
constexpr float Matrix3x3::operator[](int i) const
{
	return v[i];
}

constexpr void Matrix3x3::SetIdentity()
{
	v[0] = 1.0f;
	v[1] = v[2] = v[3] = 0.0f;
	v[4] = 1.0f;
	v[5] = v[6] = v[7] = 0.0f;
	v[8] = 1.0f;
}

constexpr void Matrix3x3::Transform(Vector3 & out, Vector3 const & in) const
{
	out.x = in.x * v[0] + in.y * v[3] + in.z * v[6];
	out.y = in.x * v[1] + in.y * v[4] + in.z * v[7];
	out.z = in.x * v[2] + in.y * v[5] + in.z * v[8];
}

inline void Matrix3x3::Transpose()
{
	for (int i = 0; i < 2; ++i) {
		for (int j = i+1; j < 3; ++j) {
			std::swap(v[i*4+j], v[j*4+i]);
		}
	}
}

inline Matrix3x3 Matrix3x3::GetInverse() const
{
	Matrix3x3 ret;
	bx::mtx3Inverse(ret.v, v);
	return ret;
}


template <typename T>
constexpr void TRect<T>::Set(TRect<T>::DataType const & _min, TRect<T>::DataType const & _size)
{
	rMin = _min;
	rMax.Set(_min.x + _size.x, _min.y + _size.y);
}

template <typename T>
constexpr T TRect<T>::Width() const
{
	return rMax.x - rMin.x;
}
template <typename T>
constexpr T TRect<T>::Height() const
{
	return rMax.y - rMin.y;
}

template <typename T>
constexpr typename TRect<T>::DataType TRect<T>::Size() const
{
	return DataType{ Width(), Height() };
}
template <typename T>
constexpr typename TRect<T>::DataType TRect<T>::Center() const
{
	assert(!IsEmpty());
	return (rMin + rMax) / 2;
}

template <typename T>
constexpr bool TRect<T>::IsEmpty() const
{
	return (rMin.x > rMax.x) || (rMin.y > rMax.y);
}

template <typename T>
constexpr void TRect<T>::Union(TRect<T> const & b)
{
	rMin.Set(std::min(rMin.x, b.rMin.x), std::min(rMin.y, b.rMin.y));
	rMax.Set(std::max(rMax.x, b.rMax.x), std::max(rMax.y, b.rMax.y));
}

template <typename T>
constexpr void TRect<T>::Scale(float s)
{
	DataType const & sw = Size() * (s * 0.5f);
	rMin -= sw;
	rMax += sw;
}

template <typename T>
constexpr bool TRect<T>::operator==(TRect<T> const & other) const
{
	return rMin == other.rMin && rMax == other.rMax;
}

template <typename T>
constexpr bool TRect<T>::Overlap(TRect<T> const & other) const
{
	return !IsEmpty()
		&& Overlap1D(rMin.x, rMax.x, other.rMin.x, other.rMax.x)
		&& Overlap1D(rMin.y, rMax.y, other.rMin.y, other.rMax.y);
}

template <typename T>
constexpr bool TRect<T>::Conatain(TRect<T> const & other) const
{
	return !IsEmpty() && !other.IsEmpty()
		&& Contain1D(rMin.x, rMax.x, other.rMin.x, other.rMax.x)
		&& Contain1D(rMin.y, rMax.y, other.rMin.y, other.rMax.y);
}


constexpr void Box::Set(Vector3 const & _min, Vector3 const & _size)
{
	bMin = _min;
	bMax.Set(_min.x + _size.x, _min.y + _size.y, _min.z + _size.z);
}

constexpr float Box::Width() const
{
	return bMax.x - bMin.x;
}
constexpr float Box::Height() const
{
	return bMax.y - bMin.y;
}
constexpr float Box::Depth() const
{
	return bMax.z - bMin.z;
}

constexpr Vector3 Box::Size() const
{
	return Vector3{ Width(), Height(), Depth() };
}
inline Vector3 Box::Center() const
{
	assert(!IsEmpty());
	return (bMin + bMax) / 2;
}

constexpr bool Box::IsEmpty() const
{
	return (bMin.x > bMax.x) || (bMin.y > bMax.y) || (bMin.z > bMax.z);
}

constexpr void Box::Union(Box const & b)
{
	bMin.Set(std::min(bMin.x, b.bMin.x), std::min(bMin.y, b.bMin.y), std::min(bMin.z, b.bMin.z));
	bMax.Set(std::max(bMax.x, b.bMax.x), std::max(bMax.y, b.bMax.y), std::max(bMax.z, b.bMax.z));
}

inline void Box::Scale(float s)
{
	auto const & sw = Size() * (s * 0.5f);
	bMin -= sw;
	bMax += sw;
}

constexpr bool Box::operator==(Box const & other) const
{
	return bMin == other.bMin && bMax == other.bMax;
}

constexpr bool Box::Overlap(Box const & other) const
{
	return !IsEmpty()
		&& Overlap1D(bMin.x, bMax.x, other.bMin.x, other.bMax.x)
		&& Overlap1D(bMin.y, bMax.y, other.bMin.y, other.bMax.y)
		&& Overlap1D(bMin.z, bMax.z, other.bMin.z, other.bMax.z);
}


constexpr bool Box::Conatain(Box const & other) const
{
	return !IsEmpty() && !other.IsEmpty()
		&& Contain1D(bMin.x, bMax.x, other.bMin.x, other.bMax.x)
		&& Contain1D(bMin.y, bMax.y, other.bMin.y, other.bMax.y)
		&& Contain1D(bMin.z, bMax.z, other.bMin.z, other.bMax.z);
}

}

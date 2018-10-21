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
	TVec2<T> ret;
	ret.Set(v[0] + other.v[0], v[1] + other.v[1]);
	return std::move(ret);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator-(TVec2 const & other) const
{
	TVec2 ret;
	ret.Set(v[0] - other.v[0], v[1] - other.v[1]);
	return std::move(ret);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator*(TVec2 const & other) const
{
	TVec2<T> ret;
	ret.Set(v[0] * other.v[0], v[1] * other.v[1]);
	return std::move(ret);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator*(T s) const
{
	TVec2 ret;
	ret.Set(v[0] * s, v[1] * s);
	return std::move(ret);
}

template <typename T>
constexpr TVec2<T> TVec2<T>::operator/(T s) const
{
	return TVec2<T>{ v[0] / s, v[1] / s };
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
constexpr void TVec2<T>::operator/=(TVec2 const & other)
{
	Set(v[0] / other.v[0], v[1] / other.v[1]);
}

template <typename T>
constexpr void TVec2<T>::operator*=(T s)
{
	Set(v[0] * s, v[1] * s);
}

template <typename T>
constexpr void TVec2<T>::operator/=(T s)
{
	set(v[0] / s, v[1] / s);
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
	return bx::sqrt(Dot(*this));
}

template <typename T>
inline T TVec2<T>::Normalize()
{
	float const len = Length();
	float const invLen = 1.0f / len;
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
inline TVec3<T> TVec3<T>::operator+(TVec3 const & other) const
{
	TVec3<T> ret;
	bx::vec3Add(ret.v, v, other.v);
	return std::move(ret);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator-(TVec3 const & other) const
{
	TVec3<T> ret;
	bx::vec3Sub(ret.v, v, other.v);
	return std::move(ret);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator*(TVec3 const & other) const
{
	TVec3<T> ret;
	bx::vec3Mul(ret.v, v, other.v);
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
inline void TVec3<T>::operator+=(TVec3 const & other)
{
	bx::vec3Add(v, v, other.v);
}

template <typename T>
inline void TVec3<T>::operator-=(TVec3 const & other)
{
	bx::vec3Sub(v, v, other.v);
}

template <typename T>
inline void TVec3<T>::operator*=(TVec3 const & other)
{
	bx::vec3Mul(v, v, other.v);
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
inline void TVec3<T>::TransformBy(Matrix3x3 const & m)
{
	T nX = x * m[0] + y * m[3] + z * m[6];
	T nY = x * m[1] + y * m[4] + z * m[7];
	T nZ = x * m[2] + y * m[5] + z * m[8];
	set(nX, nY, nZ);
}

template <typename T>
inline void TVec3<T>::TransformBy(Matrix4x4 const & m)
{
	TVec3<T> result;
	bx::vec3MulMtx(result.v, v, m.v);
	*this = result;
}

template <typename T>
constexpr T TVec3<T>::Dot(TVec3 const & other) const
{
	return bx::vec3Dot(v, other.v);
}

template <typename T>
constexpr T TVec3<T>::Length() const
{
	return bx::vec3Length(v);
}

template <typename T>
constexpr T TVec3<T>::Normalize()
{
	return bx::vec3Norm(v, v);
}

template <typename T>
inline TVec3<T> TVec3<T>::Cross(TVec3 const & other) const
{
	TVec3<T> ret;
	bx::vec3Cross(ret.v, v, other.v);
	return std::move(ret);
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
inline TVec3<T> & TVec4<T>::Vec3()
{
	return *reinterpret_cast<TVec3<T>*>(v);
}

template <typename T>
inline TVec3<T> const & TVec4<T>::Vec3() const
{
	return *reinterpret_cast<TVec3<T> const *>(v);
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
constexpr void TVec4<T>::TransformBy(Matrix4x4 const & m)
{
	TVec4<T> result;
	bx::vec4MulMtx(result.v, v, m.v);
	*this = result;
}


constexpr float& Matrix4x4::operator[](int i)
{
	return v[i];
}
constexpr float Matrix4x4::operator[](int i) const
{
	return v[i];
}

inline void Matrix4x4::SetIdentity()
{
	memset(v, 0, sizeof(v));
	v[0] = v[5] = v[10] = v[15] = 1.0f;
}

inline void Matrix4x4::SetSRT(Vector3 const & s, Vector3 const & r, Vector3 const & t)
{
	bx::mtxSRT(v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
}

inline void Matrix4x4::Transform(Vector4 & out, Vector4 const & in) const
{
	bx::vec4MulMtx(out.v, in.v, v);
}

inline void Matrix4x4::TransformPos(Vector3 & out, Vector3 const & in) const
{
	bx::vec3MulMtx(out.v, in.v, v);
}

inline void Matrix4x4::TransformDir(Vector3 & out, Vector3 const & in) const
{
	bx::vec3MulMtxXyz0(out.v, in.v, v);
}

inline void Matrix4x4::TransformPos(Vector4 & out, Vector4 const & in) const
{
	bx::vec3MulMtx(out.Vec3().v, in.Vec3().v, v);
	out.w = in.w;
}

inline void Matrix4x4::TransformDir(Vector4 & out, Vector4 const & in) const
{
	bx::vec3MulMtxXyz0(out.Vec3().v, in.Vec3().v, v);
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

constexpr void Matrix4x4::Transpose()
{
	for (int i = 0; i < 3; ++i) {
		for (int j = i+1; j < 4; ++j) {
			std::swap(v[i*4+j], v[j*4+i]);
		}
	}
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

inline void Matrix3x3::SetIdentity()
{
	memset(v, 0, sizeof(v));
	v[0] = v[4] = v[8] = 1.0f;
}

constexpr void Matrix3x3::Transform(Vector3 & out, Vector3 const & in) const
{
	out.x = in.x * v[0] + in.y * v[3] + in.z * v[6];
	out.y = in.x * v[1] + in.y * v[4] + in.z * v[7];
	out.z = in.x * v[2] + in.y * v[5] + in.z * v[8];
}

constexpr void Matrix3x3::Transpose()
{
	for (int i = 0; i < 2; ++i) {
		for (int j = i+1; j < 3; ++j) {
			std::swap(v[i*4+j], v[j*4+i]);
		}
	}
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

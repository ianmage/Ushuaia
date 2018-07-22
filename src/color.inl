#include <cmath>


namespace Ushuaia
{

constexpr void Color3F::Set(float _v0, float _v1, float _v2)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2;
}

constexpr bool Color3F::operator==(Color3F const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2];
}

inline Color3F Color3F::operator+(Color3F const & other) const
{
	Color3F ret;
	bx::vec3Add(ret.v, v, other.v);
	return std::move(ret);
}

inline Color3F Color3F::operator-(Color3F const & other) const
{
	Color3F ret;
	bx::vec3Sub(ret.v, v, other.v);
	return std::move(ret);
}

inline Color3F Color3F::operator*(float s) const
{
	Color3F ret;
	bx::vec3Mul(ret.v, v, s);
	return std::move(ret);
}

inline Color3F Color3F::operator/(float s) const
{
	Color3F ret;
	bx::vec3Mul(ret.v, v, 1.f/s);
	return std::move(ret);
}

inline void Color3F::operator+=(Color3F const & other)
{
	bx::vec3Add(v, v, other.v);
}

inline void Color3F::operator-=(Color3F const & other)
{
	bx::vec3Sub(v, v, other.v);
}

inline void Color3F::operator*=(float m)
{
	bx::vec3Mul(v, v, m);
}

inline void Color3F::operator/=(float d)
{
	bx::vec3Mul(v, v, 1.f/d);
}


inline Color3F & Color4F::Color3()
{
	return *reinterpret_cast<Color3F*>(v);
}

inline Color3F const & Color4F::Color3() const
{
	return *reinterpret_cast<Color3F const *>(v);
}

constexpr void Color4F::Set(float _v0, float _v1, float _v2, float _v3)
{
	v[0] = _v0; v[1] = _v1; v[2] = _v2; v[3] = _v3;
}

constexpr bool Color4F::operator==(Color4F const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1]
		&& v[2] == other.v[2] && v[3] == other.v[3];
}


constexpr Color4F ConvertToColorF(uint32_t const iVal)
{
	Color4F c { 0, 0, 0, 0 };
	uint32_t iR = iVal >> 24;
	uint32_t iG = (iVal >> 16) & 0xFF;
	uint32_t iB = (iVal >> 8) & 0xFF;
	uint32_t iA = iVal & 0xFF;
	c.r = static_cast<float>(iR);
	c.g = static_cast<float>(iG);
	c.b = static_cast<float>(iB);
	c.a = static_cast<float>(iA);
	return std::move(c);
}


constexpr uint32_t ConvertToColorI(Color4F const & c)
{
	uint32_t const minI = 0;
	uint32_t const maxI = 255;
	uint32_t iR = Clamp(static_cast<uint32_t>(c.r), minI, maxI);
	uint32_t iG = Clamp(static_cast<uint32_t>(c.g), minI, maxI);
	uint32_t iB = Clamp(static_cast<uint32_t>(c.b), minI, maxI);
	uint32_t iA = Clamp(static_cast<uint32_t>(c.a), minI, maxI);
	return (iR << 24) & (iG << 16) & (iB << 8) & iA;
}


inline void ToLinear(Color3F & cOut, Color3F const & cIn)
{
	cOut.r = ::powf(cIn.r, 2.2f);
	cOut.g = ::powf(cIn.g, 2.2f);
	cOut.b = ::powf(cIn.b, 2.2f);
}

inline void ToGamma(Color3F & cOut, Color3F const & cIn)
{
	cOut.r = ::powf(cIn.r, 1.f/2.2f);
	cOut.g = ::powf(cIn.g, 1.f/2.2f);
	cOut.b = ::powf(cIn.b, 1.f/2.2f);
}

inline void ToLinearAccurate(Color3F & cOut, Color3F const & cIn)
{
	for (uint8_t i = 0; i < 3; ++i) {
		cOut.v[i] = (cIn.v[i] <= 0.04045f) ?
			cIn.v[i] / 12.92f :
			::powf( (cIn.v[i] + 0.055f) / 1.055f, 2.4f );
	}
}

inline void ToGammaAccurate(Color3F & cOut, Color3F const & cIn)
{
	for (uint8_t i = 0; i < 3; ++i) {
		cOut.v[i] = (cIn.v[i] <= 0.0031308f) ?
			cIn.v[i] * 12.92f :
			::powf(cIn.v[i], 1.f/2.4f) * 1.055f - 0.055f;
	}
}

inline void ToLinear(Color4F & cOut, Color4F const & cIn)
{
	ToLinear(cOut.Color3(), cIn.Color3());
	cOut.a = cIn.a;
}
inline void ToGamma(Color4F & cOut, Color4F const & cIn)
{
	ToGamma(cOut.Color3(), cIn.Color3());
	cOut.a = cIn.a;
}
inline void ToLinearAccurate(Color4F & cOut, Color4F const & cIn)
{
	ToLinearAccurate(cOut.Color3(), cIn.Color3());
	cOut.a = cIn.a;
}
inline void ToGammaAccurate(Color4F & cOut, Color4F const & cIn)
{
	ToGammaAccurate(cOut.Color3(), cIn.Color3());
	cOut.a = cIn.a;
}

}

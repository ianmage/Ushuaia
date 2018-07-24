#include <cmath>


namespace Ushuaia
{

constexpr void Color3F::Set(float _r, float _g, float _b)
{
	r = _r; g = _g; b = _b;
}

constexpr void Color3F::SetUint(uint8_t _r, uint8_t _g, uint8_t _b)
{
	r = _r / 255.f;
	g = _g / 255.f;
	b = _b / 255.f;
}

constexpr bool Color3F::operator==(Color3F const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2];
}

inline Color3F Color3F::operator+(Color3F const & other) const
{
	Color3F ret;
	bx::vec3Add(ret.v, v, other.v);
	return ret;
}

inline Color3F Color3F::operator-(Color3F const & other) const
{
	Color3F ret;
	bx::vec3Sub(ret.v, v, other.v);
	return ret;
}

inline Color3F Color3F::operator*(float s) const
{
	Color3F ret;
	bx::vec3Mul(ret.v, v, s);
	return ret;
}

inline Color3F Color3F::operator/(float s) const
{
	Color3F ret;
	bx::vec3Mul(ret.v, v, 1.f/s);
	return ret;
}

inline void Color3F::operator+=(Color3F const & other)
{
	bx::vec3Add(v, v, other.v);
}

inline void Color3F::operator-=(Color3F const & other)
{
	bx::vec3Sub(v, v, other.v);
}

inline void Color3F::operator*=(float s)
{
	bx::vec3Mul(v, v, s);
}

inline void Color3F::operator/=(float s)
{
	bx::vec3Mul(v, v, 1.f/s);
}


inline Color3F & Color4F::Color3()
{
	return *reinterpret_cast<Color3F*>(v);
}

inline Color3F const & Color4F::Color3() const
{
	return *reinterpret_cast<Color3F const *>(v);
}

constexpr void Color4F::Set(float _r, float _g, float _b, float _a)
{
	r = _r; g = _g; b = _b; a = _a;
}

constexpr void Color4F::SetUint(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
{
	r = _r / 255.f;
	g = _g / 255.f;
	b = _b / 255.f;
	a = _a / 255.f;
}

constexpr bool Color4F::operator==(Color4F const & other) const
{
	return v[0] == other.v[0] && v[1] == other.v[1]
		&& v[2] == other.v[2] && v[3] == other.v[3];
}

inline Color4F Color4F::operator*(float s) const
{
	return {{r * s, g * s, b * s, a * s}};
}

inline Color4F Color4F::operator/(float s) const
{
	s = 1.f / s;
	return {{r * s, g * s, b * s, a * s}};
}

inline void Color4F::operator*=(float s)
{
	r *= s;
	g *= s;
	b *= s;
	a *= s;
}

inline void Color4F::operator/=(float s)
{
	s = 1.f / s;
	r *= s;
	g *= s;
	b *= s;
	a *= s;
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
	return c;
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

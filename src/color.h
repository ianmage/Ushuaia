#pragma once

#include "math.h"


namespace Ushuaia
{

struct Color3F
{
	union {
		float v[3];

		struct {
			float r, g, b;
		};
	};

	constexpr void Set(float _r, float _g, float _b);
	constexpr void SetUint(uint8_t _r, uint8_t _g, uint8_t _b);

	constexpr bool operator==(Color3F const & other) const;

	inline Color3F operator+(Color3F const & other) const;
	inline Color3F operator-(Color3F const & other) const;
	inline Color3F operator*(float s) const;
	inline Color3F operator/(float s) const;

	inline void operator+=(Color3F const & other);
	inline void operator-=(Color3F const & other);
	inline void operator*=(float s);
	inline void operator/=(float s);
};


struct Color4F
{
	union {
		float v[4];

		struct {
			float r, g, b, a;
		};
	};

	inline Color3F & Color3();
	inline Color3F const & Color3() const;

	constexpr void Set(float _r, float _g, float _b, float _a);
	constexpr void SetUint(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a);

	constexpr bool operator==(Color4F const & other) const;

	inline Color4F operator+(Color4F const & other) const;
	inline Color4F operator-(Color4F const & other) const;
	inline Color4F operator*(float s) const;
	inline Color4F operator/(float s) const;

	inline void operator+=(Color4F const & other);
	inline void operator-=(Color4F const & other);
	inline void operator*=(float s);
	inline void operator/=(float s);
};


typedef uint32_t	ColorI;


constexpr Color4F ConvertToColorF(uint32_t const iVal);
constexpr uint32_t ConvertToColorI(Color4F const & c);

inline void ToLinear(Color3F & cOut, Color3F const & cIn);
inline void ToGamma(Color3F & cOut, Color3F const & cIn);
inline void ToLinearAccurate(Color3F & cOut, Color3F const & cIn);
inline void ToGammaAccurate(Color3F & cOut, Color3F const & cIn);
inline void ToLinear(Color4F & cOut, Color4F const & cIn);
inline void ToGamma(Color4F & cOut, Color4F const & cIn);
inline void ToLinearAccurate(Color4F & cOut, Color4F const & cIn);
inline void ToGammaAccurate(Color4F & cOut, Color4F const & cIn);

}


#include "color.inl"

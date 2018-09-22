#pragma once

#include "color.h"
#include "bgfx/bgfx.h"
#include <vector>
#include "serialize.h"


namespace Ushuaia
{

struct AmbientLight
{
	Color4F color;
};


struct DirLight
{
	Color4F color;
	Vector4 dir;
};


struct PointLight
{
	Color4F color;
	Vector4 pos;
	Vector4 attn;
};


struct SpotLight
{
	Color4F color;
	Vector4 pos;
	Vector4 attnOuter;
	Vector4 dirInner;
};


struct Light
{
	enum Type
	{
		Ambient,
		Directional,
		Point,
		Spot,

		Count
	};

	static void Init();
	static void Fini();
	static void Clear();

	static void Serialize(JsonWriter & _writer);
	static void Deserialize(JsonValue const & _jsObj);

	static void UpdateAll(Matrix4x4 const & mtxView);

	static void Submit();

	static AmbientLight ambLight;
	static DirLight dirLight;

	static uint16_t AddPointLight(bool isSpot);

	static std::vector<PointLight> s_pointLights;
	static std::vector<SpotLight> s_spotLights;
};

}

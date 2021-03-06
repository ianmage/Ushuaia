#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "Color.h"
#include "Serialize.h"
#include "Camera.h"


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
	Color4F color;	// rgb, attn
	Vector4 pos;	// xyz, range
};


struct SpotLight
{
	Color4F color;
	Vector4 pos;	// xyz, range
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

	static void UpdateAll(Camera *pCam);

	static void Submit();

	static AmbientLight ambLight;
	static DirLight dirLight;

	static uint16_t AddPointLight(bool isSpot);

	static std::vector<PointLight> s_pointLights;
	static std::vector<SpotLight> s_spotLights;
	static std::vector<PointLight> s_visiblePointLights;
	static std::vector<SpotLight> s_visibleSpotLights;
};

}

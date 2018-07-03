#pragma once

#include "color.h"
#include "bgfx/bgfx.h"
#include <vector>


namespace My3D
{
	struct AmbientLight
	{
		Color4F color;
	};


	struct DirLight
	{
		Color4F color;
		Vector4 dir;

		void calcViewParam(Matrix4x4 const & mtxView);

	private:
		Vector4 dirVS;
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

		static void init();
		static void clearAll();
		static void updateAll(Matrix4x4 const & mtxView);

		static void submit();

		static AmbientLight ambLight;
		static DirLight dirLight;

		static uint16_t addPointLight(bool isSpot);

	protected:

		static std::vector<PointLight> s_pointLights;
		static std::vector<SpotLight> s_spotLights;
	};

}

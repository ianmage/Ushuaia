#include "light.h"
#include "shader.h"


namespace Ushuaia
{

	static bgfx::UniformHandle uhLightsCnt;
	static bgfx::UniformHandle uhAmbientColor;
	static bgfx::UniformHandle uhDirectionalColor;
	static bgfx::UniformHandle uhDirectionalDir;
	static bgfx::UniformHandle uhPointColor;
	static bgfx::UniformHandle uhPointPos;	// w for spot Outer
	static bgfx::UniformHandle uhPointAttnRange;
	static bgfx::UniformHandle uhSpotDirInner;


	decltype(Light::ambLight) Light::ambLight;
	decltype(Light::dirLight) Light::dirLight;
	decltype(Light::s_pointLights) Light::s_pointLights;
	decltype(Light::s_spotLights) Light::s_spotLights;


	uint8_t const MAX_POINT_LIGHT = 6;
	uint8_t const MAX_SPOT_LIGHT = 2;


	static Vector4 s_dirLightDir;
	static std::vector<Color4F> s_lightColorBuf;
	static std::vector<Vector4> s_lightPosBuf;
	static std::vector<Vector4> s_lightAttnRangeBuf;
	static std::vector<Vector4> s_spotDirInnerBuf;
	static Vector4 s_lightsCnt;


	void Light::Init()
	{
		uhLightsCnt = Shader::AddUniform("u_lightsCnt", bgfx::UniformType::Vec4);
		uhAmbientColor = Shader::AddUniform("u_lightAmbColor", bgfx::UniformType::Vec4);
		uhDirectionalColor = Shader::AddUniform("u_lightDirColor", bgfx::UniformType::Vec4);
		uhDirectionalDir = Shader::AddUniform("u_lightDirDir", bgfx::UniformType::Vec4);
		uhPointColor = Shader::AddUniform("u_lightColor", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhPointPos = Shader::AddUniform("u_lightPos", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhPointAttnRange = Shader::AddUniform("u_lightAttnRange", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhSpotDirInner = Shader::AddUniform("u_lightSpotDirInner", bgfx::UniformType::Vec4, MAX_SPOT_LIGHT);
	}


	void Light::ClearAll()
	{
		s_lightsCnt.Set(0, 0, 0, 0);
		s_lightColorBuf.clear();
		s_lightPosBuf.clear();
		s_lightAttnRangeBuf.clear();
		s_spotDirInnerBuf.clear();

		ambLight.color.Set(0, 0, 0, 0);
		dirLight.color.Set(0, 0, 0, 0);
		dirLight.color.Set(0, 0, 0, 0);

		s_pointLights.clear();
		s_spotLights.clear();
	}


	void Light::UpdateAll(Matrix4x4 const & mtxView)
	{
		s_lightsCnt.Set(0, 0, 0, 0);
		s_lightColorBuf.clear();
		s_lightPosBuf.clear();
		s_lightAttnRangeBuf.clear();
		s_spotDirInnerBuf.clear();

		mtxView.TransformVec3(s_dirLightDir, dirLight.dir);
		s_dirLightDir.Vec3().Normalize();
		
		s_lightsCnt.x = s_lightsCnt.y = 1.f;

		uint8_t const plCnt = std::min(MAX_POINT_LIGHT, (uint8_t)s_pointLights.size());
		uint8_t const spCnt = std::min(MAX_SPOT_LIGHT, (uint8_t)s_spotLights.size());
		uint8_t const lightBufSize = (spCnt > 0) ? MAX_POINT_LIGHT + spCnt : plCnt;
		s_lightColorBuf.resize(lightBufSize);
		s_lightPosBuf.resize(lightBufSize);
		s_lightAttnRangeBuf.resize(lightBufSize);
		s_spotDirInnerBuf.resize(spCnt);

		for (uint8_t i = 0; i < plCnt; ++i)
		{
			auto & pl = s_pointLights[i];
			s_lightColorBuf[i] = pl.color;
			mtxView.TransformVec3(s_lightPosBuf[i], pl.pos);
			s_lightAttnRangeBuf[i] = pl.attn;
		}
		s_lightsCnt.z = plCnt;

		for (uint8_t i = 0; i < spCnt; ++i)
		{
			auto & sl = s_spotLights[i];
			uint8_t const bufIdx = MAX_POINT_LIGHT + i;
			s_lightColorBuf[bufIdx] = sl.color;
			mtxView.TransformVec3(s_lightPosBuf[bufIdx], sl.pos);
			s_lightAttnRangeBuf[bufIdx] = sl.attnOuter;
			mtxView.TransformVec3(s_spotDirInnerBuf[i], sl.dirInner);
			s_spotDirInnerBuf[i].Vec3().Normalize();
		}
		s_lightsCnt.w = spCnt;
	}


	uint16_t Light::AddPointLight(bool isSpot)
	{
		size_t ret = 0;
		if (!isSpot)
		{
			ret = s_pointLights.size();
			s_pointLights.emplace_back();
		}
		else
		{
			ret = s_spotLights.size();
			s_spotLights.emplace_back();
		}
		return static_cast<uint16_t>(ret);
	}


	void Light::Submit()
	{
		bgfx::setUniform(uhLightsCnt, &s_lightsCnt);

		bgfx::setUniform(uhAmbientColor, &ambLight.color);

		bgfx::setUniform(uhDirectionalColor, &dirLight.color);
		bgfx::setUniform(uhDirectionalDir, &s_dirLightDir);

		uint8_t const plCnt = std::min(MAX_POINT_LIGHT, (uint8_t)s_pointLights.size());
		uint8_t const spCnt = std::min(MAX_SPOT_LIGHT, (uint8_t)s_spotLights.size());
		uint8_t const lightBufSize = (spCnt > 0) ? MAX_POINT_LIGHT + spCnt : plCnt;

		bgfx::setUniform(uhPointColor, s_lightColorBuf.data(), lightBufSize);
		bgfx::setUniform(uhPointPos, s_lightPosBuf.data(), lightBufSize);
		bgfx::setUniform(uhPointAttnRange, s_lightAttnRangeBuf.data(), lightBufSize);
		bgfx::setUniform(uhSpotDirInner, s_spotDirInnerBuf.data(), spCnt);
	}

}

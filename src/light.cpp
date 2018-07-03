#include "light.h"
#include "mathHelper.h"
#include "shader.h"


namespace My3D
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


	static std::vector<Color4F> s_lightColorBuf;
	static std::vector<Vector4> s_lightPosBuf;
	static std::vector<Vector4> s_lightAttnRangeBuf;
	static std::vector<Vector4> s_spotDirInnerBuf;
	static Vector4 s_lightsCnt;


	void Light::init()
	{
		uhLightsCnt = Shader::addUniform("u_lightsCnt", bgfx::UniformType::Vec4);
		uhAmbientColor = Shader::addUniform("u_lightAmbColor", bgfx::UniformType::Vec4);
		uhDirectionalColor = Shader::addUniform("u_lightDirColor", bgfx::UniformType::Vec4);
		uhDirectionalDir = Shader::addUniform("u_lightDirDir", bgfx::UniformType::Vec4);
		uhPointColor = Shader::addUniform("u_lightColor", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhPointPos = Shader::addUniform("u_lightPos", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhPointAttnRange = Shader::addUniform("u_lightAttnRange", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
		uhSpotDirInner = Shader::addUniform("u_lightSpotDirInner", bgfx::UniformType::Vec4, MAX_SPOT_LIGHT);
	}


	void Light::clearAll()
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


	void Light::updateAll(Matrix4x4 const & mtxView)
	{
		s_lightsCnt.Set(0, 0, 0, 0);
		s_lightColorBuf.clear();
		s_lightPosBuf.clear();
		s_lightAttnRangeBuf.clear();
		s_spotDirInnerBuf.clear();

		dirLight.calcViewParam(mtxView);
		s_lightsCnt.x = s_lightsCnt.y = 1.f;

		uint8_t const plCnt = std::min(MAX_POINT_LIGHT , (uint8_t)s_pointLights.size());
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
			math::vec4MulMtxComp3(s_lightPosBuf[i], pl.pos, mtxView);
			s_lightAttnRangeBuf[i] = pl.attn;
		}
		s_lightsCnt.z = plCnt;

		for (uint8_t i = 0; i < spCnt; ++i)
		{
			auto & sl = s_spotLights[i];
			uint8_t const bufIdx = MAX_POINT_LIGHT + i;
			s_lightColorBuf[bufIdx] = sl.color;
			math::vec4MulMtxComp3(s_lightPosBuf[bufIdx], sl.pos, mtxView);
			s_lightAttnRangeBuf[bufIdx] = sl.attnOuter;
			math::vec4MulMtxComp3(s_spotDirInnerBuf[i], sl.dirInner, mtxView);
		}
		s_lightsCnt.w = spCnt;
	}


	uint16_t Light::addPointLight(bool isSpot)
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


	void Light::submit()
	{
		bgfx::setUniform(uhLightsCnt, &s_lightsCnt);

		bgfx::setUniform(uhAmbientColor, &ambLight.color);

		bgfx::setUniform(uhDirectionalColor, &dirLight.color);
		bgfx::setUniform(uhDirectionalDir, &dirLight.dir);

		uint8_t const plCnt = std::min(MAX_POINT_LIGHT, (uint8_t)s_pointLights.size());
		uint8_t const spCnt = std::min(MAX_SPOT_LIGHT, (uint8_t)s_spotLights.size());
		uint8_t const lightBufSize = (spCnt > 0) ? MAX_POINT_LIGHT + spCnt : plCnt;

		bgfx::setUniform(uhPointColor, s_lightColorBuf.data(), lightBufSize);
		bgfx::setUniform(uhPointPos, s_lightPosBuf.data(), lightBufSize);
		bgfx::setUniform(uhPointAttnRange, s_lightAttnRangeBuf.data(), lightBufSize);
		bgfx::setUniform(uhSpotDirInner, s_spotDirInnerBuf.data(), spCnt);
	}


	void DirLight::calcViewParam(Matrix4x4 const & mtxView)
	{
		math::vec4MulMtxComp3(dirVS, dir, mtxView);
	}

}

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


static Color4F s_ambLightColor;
static Color4F s_dirLightColor;
static Vector4 s_dirLightDir;
static std::vector<Color4F> s_lightColorBuf;
static std::vector<Vector4> s_lightPosBuf;
static std::vector<Vector4> s_lightAttnRangeBuf;
static std::vector<Vector4> s_spotDirInnerBuf;
static Vector4 s_lightsCnt;


void Light::Init()
{
	uhLightsCnt = bgfx::createUniform("PV_lightsCnt", bgfx::UniformType::Vec4);
	uhAmbientColor = bgfx::createUniform("PV_lightAmbColor", bgfx::UniformType::Vec4);
	uhDirectionalColor = bgfx::createUniform("PV_lightDirColor", bgfx::UniformType::Vec4);
	uhDirectionalDir = bgfx::createUniform("PV_lightDirDir", bgfx::UniformType::Vec4);
	uhPointColor = bgfx::createUniform("PV_lightColor", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhPointPos = bgfx::createUniform("PV_lightPos", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhPointAttnRange = bgfx::createUniform("PV_lightAttnRange", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhSpotDirInner = bgfx::createUniform("PV_lightSpotDirInner", bgfx::UniformType::Vec4, MAX_SPOT_LIGHT);
}


void Light::Fini()
{
	bgfx::destroy(uhLightsCnt);
	bgfx::destroy(uhAmbientColor);
	bgfx::destroy(uhDirectionalColor);
	bgfx::destroy(uhDirectionalDir);
	bgfx::destroy(uhPointColor);
	bgfx::destroy(uhPointPos);
	bgfx::destroy(uhPointAttnRange);
	bgfx::destroy(uhSpotDirInner);
}


void Light::Clear()
{
	s_lightsCnt.Set(0, 0, 0, 0);
	s_lightColorBuf.clear();
	s_lightPosBuf.clear();
	s_lightAttnRangeBuf.clear();
	s_spotDirInnerBuf.clear();

	ambLight.color.Set(0, 0, 0, 0);
	dirLight.color.Set(0, 0, 0, 0);

	s_pointLights.clear();
	s_spotLights.clear();
}


void Light::Serialize(JsonWriter & _writer)
{
	_writer.StartObject();

	_writer.Key("Ambient");
	WriteFloatArray(_writer, ambLight.color.v, 4);

	_writer.Key("Directional");
	_writer.StartObject();
	_writer.Key("Color");
	WriteFloatArray(_writer, dirLight.color.v, 4);
	_writer.Key("Dir");
	WriteFloatArray(_writer, dirLight.dir.v, 4);
	_writer.EndObject();

	_writer.Key("Point");
	_writer.StartArray();
	for (auto const & pl : s_pointLights) {
		_writer.StartObject();
		_writer.Key("Color");
		WriteFloatArray(_writer, pl.color.v, 4);
		_writer.Key("Pos");
		WriteFloatArray(_writer, pl.pos.v, 4);
		_writer.Key("Attenuation");
		WriteFloatArray(_writer, pl.attn.v, 4);
		_writer.EndObject();
	}
	_writer.EndArray();

	_writer.Key("Spot");
	_writer.StartArray();
	for (auto const & spl : s_spotLights) {
		_writer.StartObject();
		_writer.Key("Color");
		WriteFloatArray(_writer, spl.color.v, 4);
		_writer.Key("Pos");
		WriteFloatArray(_writer, spl.pos.v, 4);
		_writer.Key("AttnOuter");
		WriteFloatArray(_writer, spl.attnOuter.v, 4);
		_writer.Key("DirInner");
		WriteFloatArray(_writer, spl.dirInner.v, 4);
		_writer.EndObject();
	}
	_writer.EndArray();
	
	_writer.EndObject();
}


void Light::Deserialize(JsonValue const & _jsObj)
{
	Clear();
	JsonValue::ConstMemberIterator itr;

	itr = _jsObj.FindMember("Ambient");
	if (itr != _jsObj.MemberEnd())
		ReadFloatArray(itr->value, ambLight.color.v);

	itr = _jsObj.FindMember("Directional");
	if (itr != _jsObj.MemberEnd()) {
		JsonValue::ConstMemberIterator dirItr;
		auto const & dirObj = itr->value;
		dirItr = dirObj.FindMember("Color");
		if (dirItr != dirObj.MemberEnd())
			ReadFloatArray(dirItr->value, dirLight.color.v);
		dirItr = dirObj.FindMember("Dir");
		if (dirItr != dirObj.MemberEnd())
			ReadFloatArray(dirItr->value, dirLight.dir.v);
	}

	itr = _jsObj.FindMember("Point");
	if (itr != _jsObj.MemberEnd()) {
		int i = 0;
		for (auto const & ptObj : itr->value.GetArray()) {
			JsonValue::ConstMemberIterator ptItr;
			auto & pl = s_pointLights[i++];
			ptItr = ptObj.FindMember("Color");
			if (ptItr != ptObj.MemberEnd())
				ReadFloatArray(ptItr->value, pl.color.v);
			ptItr = ptObj.FindMember("Pos");
			if (ptItr != ptObj.MemberEnd())
				ReadFloatArray(ptItr->value, pl.pos.v);
			ptItr = ptObj.FindMember("Attenuation");
			if (ptItr != ptObj.MemberEnd())
				ReadFloatArray(ptItr->value, pl.attn.v);
		}
	}

	itr = _jsObj.FindMember("Spot");
	if (itr != _jsObj.MemberEnd()) {
		int i = 0;
		for (auto const & spObj : itr->value.GetArray()) {
			JsonValue::ConstMemberIterator spItr;
			auto & sl = s_spotLights[i++];
			spItr = spObj.FindMember("Color");
			if (spItr != spObj.MemberEnd())
				ReadFloatArray(spItr->value, sl.color.v);
			spItr = spObj.FindMember("Pos");
			if (spItr != spObj.MemberEnd())
				ReadFloatArray(spItr->value, sl.pos.v);
			spItr = spObj.FindMember("AttnOuter");
			if (spItr != spObj.MemberEnd())
				ReadFloatArray(spItr->value, sl.attnOuter.v);
			spItr = spObj.FindMember("DirInner");
			if (spItr != spObj.MemberEnd())
				ReadFloatArray(spItr->value, sl.dirInner.v);
		}
	}

#if 0
	itr = _jsObj.FindMember("Model");
	if (itr != _jsObj.MemberEnd()) {
		pModel = Model::Load(itr->value.GetString());
	}

	itr = _jsObj.FindMember("Transform");
	if (itr != _jsObj.MemberEnd()) {
		int i = 0;
		for (auto & v : itr->value.GetArray()) {
			transform[i++] = v.GetFloat();
		}
	}
#endif
}


void Light::UpdateAll(Matrix4x4 const & mtxView)
{
	s_lightsCnt.Set(0, 0, 0, 0);
	s_lightColorBuf.clear();
	s_lightPosBuf.clear();
	s_lightAttnRangeBuf.clear();
	s_spotDirInnerBuf.clear();

	ToLinearAccurate(s_ambLightColor, ambLight.color);

	ToLinearAccurate(s_dirLightColor, dirLight.color);
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
		ToLinearAccurate(s_lightColorBuf[i], pl.color);
		mtxView.TransformVec3(s_lightPosBuf[i], pl.pos);
		s_lightAttnRangeBuf[i] = pl.attn;
	}
	s_lightsCnt.z = plCnt;

	for (uint8_t i = 0; i < spCnt; ++i)
	{
		auto & sl = s_spotLights[i];
		uint8_t const bufIdx = MAX_POINT_LIGHT + i;
		ToLinearAccurate(s_lightColorBuf[bufIdx], sl.color);
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

	bgfx::setUniform(uhAmbientColor, &s_ambLightColor);

	bgfx::setUniform(uhDirectionalColor, &s_dirLightColor);
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

#include "light.h"
#include "shader.h"


namespace Ushuaia
{

static bgfx::UniformHandle uhAmbientColor;
static bgfx::UniformHandle uhDirectionalColor;
static bgfx::UniformHandle uhDirectionalDir;


decltype(Light::ambLight) Light::ambLight;
decltype(Light::dirLight) Light::dirLight;
decltype(Light::s_pointLights) Light::s_pointLights;
decltype(Light::s_spotLights) Light::s_spotLights;
decltype(Light::s_pointLights) Light::s_pointLightsInView;
decltype(Light::s_spotLights) Light::s_spotLightsInView;


static Color4F s_ambLightColor;
static Color4F s_dirLightColor;
static Vector4 s_dirLightDir;


void Light::Init()
{
	uhAmbientColor = bgfx::createUniform("PV_lightAmbColor", bgfx::UniformType::Vec4);
	uhDirectionalColor = bgfx::createUniform("PV_lightDirColor", bgfx::UniformType::Vec4);
	uhDirectionalDir = bgfx::createUniform("PV_lightDirDir", bgfx::UniformType::Vec4);
}


void Light::Fini()
{
	bgfx::destroy(uhAmbientColor);
	bgfx::destroy(uhDirectionalColor);
	bgfx::destroy(uhDirectionalDir);
}


void Light::Clear()
{
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


void Light::UpdateAll(Camera *pCam)
{
	Matrix4x4 const & mtxView = pCam->mtxView;
	ToLinearAccurate(s_ambLightColor, ambLight.color);

	ToLinearAccurate(s_dirLightColor, dirLight.color);

	mtxView.TransformVec3(s_dirLightDir, dirLight.dir);
	s_dirLightDir.Vec3().Normalize();

	s_pointLightsInView.clear();
	s_spotLightsInView.clear();
	// culling
	for (auto const & pl : s_pointLights) {
		if (pCam->IsVisible(pl.pos))
			s_pointLightsInView.push_back(pl);
	}
	for (auto const & pl : s_spotLights) {
		if (pCam->IsVisible(pl.pos))
			s_spotLightsInView.push_back(pl);
	}
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
	bgfx::setUniform(uhAmbientColor, &s_ambLightColor);

	bgfx::setUniform(uhDirectionalColor, &s_dirLightColor);
	bgfx::setUniform(uhDirectionalDir, &s_dirLightDir);
}

}

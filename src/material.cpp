#include "material.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"

#pragma optimize("", off)


namespace Ushuaia
{

decltype(Material::s_mtls) Material::s_mtls;


Material::Material(std::string const & _name)
	: name_(_name), pShader_(nullptr)
{
}


Material* Material::Load(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());

	auto itr = s_mtls.find(k);
	if (itr != s_mtls.end())
		return itr->second;

	Material *pMtl = new Material(_name);
	s_mtls.emplace(k, pMtl);

	pMtl->Deserialize();

	return pMtl;
}


void Material::ClearAll()
{
	for (auto & m : s_mtls) {
		delete m.second;
	}
	s_mtls.clear();
}


void Material::Serialize() const
{
	JsonWriter writer;

	if (pShader_) {
		writer.Key("Shader");
		writer.StartArray();
		writer.String(pShader_->VsName());
		writer.String(pShader_->FsName());
		writer.EndArray();
	}

	writer.Key("RenderState");
	writer.String(NumToAry79Str(renderStates));

	writer.Key("Parameters");
	writer.StartObject();
	if (!Shader::s_vec4NameMap.empty()) {
		writer.Key("Vec4");
		writer.StartObject();
		for (auto & m : Shader::s_vec4NameMap) {
			writer.Key(m.second);
			Vector4 *vec4 = GetParamVec4(m.first);
			size_t const maxFltWidth = 12;
			char buf[maxFltWidth * 4];
			int len = ftoa(vec4->v, 4, buf, 0, 4);
			writer.StartArray();
			writer.RawValue(buf, len, rapidjson::kArrayType);
			writer.EndArray();
		}
		writer.EndObject();
	}
	writer.EndObject();

	writer.Save("material/" + name_);
}


bool Material::Deserialize()
{
	JsonReader reader;
	if (!reader.Load("material/" + name_))
		return false;

	JsonValue::ConstMemberIterator itr;
	itr = reader.FindMember("Shader");
	if (itr != reader.MemberEnd()) {
		std::string vsName = itr->value[0].GetString();
		std::string fsName = itr->value[1].GetString();
		SetShader( Shader::Load(vsName, fsName) );
	}

	itr = reader.FindMember("RenderState");
	if (itr != reader.MemberEnd()) {
		renderStates = Ary79StrToNum(itr->value.GetString());
	}

	itr = reader.FindMember("Parameters");
	if (itr != reader.MemberEnd()) {
		auto const & paramMap = itr->value;
		JsonValue::ConstMemberIterator v4Itr = paramMap.FindMember("Vec4");
		if (v4Itr != paramMap.MemberEnd()) {
			for (auto & v4m : v4Itr->value.GetObject()) {
				auto const & a = v4m.value.GetArray();
				Vector4 *vec4 = GetParamVec4(RT_HASH(v4m.name.GetString()));
				for (uint8_t i = 0; i < 4; ++i) {
					vec4->v[i] = a[i].GetFloat();
				}
			}
		}
	}

	return true;
}


void Material::SetShader(Shader *_pShader)
{
	if (pShader_ == _pShader)
		return;
	if (_pShader)
	{
		paramData_.resize(_pShader->paramSize_);
	}
	else
	{
		decltype(paramData_) newPD;
		paramData_.swap(newPD);
	}
	pShader_ = _pShader;
}


void Material::Submit(uint64_t override0, uint64_t override1)
{
	assert(pShader_);
	bgfx::setState(RenderState::overrideMe(renderStates, override0, override1));

	pShader_->SetParams(paramData_.data());
}


Vector4* Material::GetParamVec4(size_t _nameKey) const
{
	assert(pShader_);
	uint16_t const idx = pShader_->ParamIndex(_nameKey);
	if (UINT16_MAX == idx)
		return nullptr;
	auto pData = const_cast<uint8_t*>(&paramData_[idx]);
	return reinterpret_cast<Vector4*>(pData);
}

}

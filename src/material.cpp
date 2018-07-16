#include "material.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"

#pragma optimize("", off)


namespace Ushuaia
{
#if 0
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

	Serialize(writer);

	writer.Save("material/" + name_);
}


bool Material::Deserialize()
{
	JsonReader reader;
	if (!reader.Load("material/" + name_))
		return false;

	Deserialize(reader);

	return true;
}
#endif

void Material::Serialize(JsonWriter & writer) const
{
	writer.StartObject();

	if (pShader_) {
		writer.Key("Shader");
		writer.StartArray();
		writer.String(pShader_->VsName());
		writer.String(pShader_->FsName());
		writer.EndArray();
	}

	writer.Key("RenderState");
	writer.String(NumToAry79Str(renderState));

	writer.Key("Parameters");
	writer.StartObject();
#if 1
	pShader_->SaveMtlParams(writer, paramData_.data());
#else
	if (!Shader::s_vec4NameMap.empty()) {
		writer.Key("Vec4");
		writer.StartObject();
		for (auto & m : Shader::s_vec4NameMap) {
			writer.Key(m.second);
			Vector4 *vec4 = GetParamVec4(m.first);
			WriteFloatArray(writer, vec4->v, 4);
		}
		writer.EndObject();
	}
#endif
	writer.EndObject();
	pShader_->SaveMtlTexs(writer, texStates);

	writer.EndObject();
}


bool Material::Deserialize(JsonValue const & jsObj)
{
	JsonValue::ConstMemberIterator itr;
	itr = jsObj.FindMember("Shader");
	if (itr != jsObj.MemberEnd()) {
		std::string vsName = itr->value[0].GetString();
		std::string fsName = itr->value[1].GetString();
		SetShader( Shader::Load(vsName, fsName) );
	}

	itr = jsObj.FindMember("RenderState");
	if (itr != jsObj.MemberEnd()) {
		renderState = Ary79StrToNum(itr->value.GetString());
	}

	itr = jsObj.FindMember("Parameters");
	if (itr != jsObj.MemberEnd()) {
		for (auto & m : itr->value.GetObject()) {
			float *pBuf = GetParam(RT_HASH(m.name.GetString()));
			ReadFloatArray(m.value, pBuf);
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
		paramData_.resize(_pShader->ParamSize());
	}
	else
	{
		decltype(paramData_) newPD;
		paramData_.swap(newPD);
	}
	pShader_ = _pShader;
}


float* Material::GetParam(size_t nameKey) const
{
	assert(pShader_);
	uint16_t const idx = pShader_->ParamIndex(nameKey);
	if (UINT16_MAX == idx)
		return nullptr;
	auto pData = const_cast<uint8_t*>(&paramData_[idx]);
	return reinterpret_cast<float*>(pData);
}

}

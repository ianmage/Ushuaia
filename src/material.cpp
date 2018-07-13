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

void Material::Serialize(JsonWriter & _writer) const
{
	if (pShader_) {
		_writer.Key("Shader");
		_writer.StartArray();
		_writer.String(pShader_->VsName());
		_writer.String(pShader_->FsName());
		_writer.EndArray();
	}

	_writer.Key("RenderState");
	_writer.String(NumToAry79Str(renderState));

	_writer.Key("Parameters");
	_writer.StartObject();
#if 1
	pShader_->SaveMtlParams(_writer, paramData_.data());
#else
	if (!Shader::s_vec4NameMap.empty()) {
		_writer.Key("Vec4");
		_writer.StartObject();
		for (auto & m : Shader::s_vec4NameMap) {
			_writer.Key(m.second);
			Vector4 *vec4 = GetParamVec4(m.first);
			WriteFloatArray(_writer, *vec4);
		}
		_writer.EndObject();
	}
#endif
	_writer.EndObject();
}


bool Material::Deserialize(JsonValue const & _jsObj)
{
	JsonValue::ConstMemberIterator itr;
	itr = _jsObj.FindMember("Shader");
	if (itr != _jsObj.MemberEnd()) {
		std::string vsName = itr->value[0].GetString();
		std::string fsName = itr->value[1].GetString();
		SetShader( Shader::Load(vsName, fsName) );
	}

	itr = _jsObj.FindMember("RenderState");
	if (itr != _jsObj.MemberEnd()) {
		renderState = Ary79StrToNum(itr->value.GetString());
	}

	itr = _jsObj.FindMember("Parameters");
	if (itr != _jsObj.MemberEnd()) {
		auto const & paramMap = itr->value;
		JsonValue::ConstMemberIterator v4Itr = paramMap.FindMember("Vec4");
		if (v4Itr != paramMap.MemberEnd()) {
			for (auto & v4m : v4Itr->value.GetObject()) {
				Vector4 *vec4 = GetParamVec4(RT_HASH(v4m.name.GetString()));
				ReadFloatArray(v4m.value, *vec4);
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
		paramData_.resize(_pShader->ParamSize());
	}
	else
	{
		decltype(paramData_) newPD;
		paramData_.swap(newPD);
	}
	pShader_ = _pShader;
}


void Material::Submit(uint64_t _overrideSt0, uint64_t _overrideSt1)
{
	bgfx::setState(RenderState::overrideMe(renderState, _overrideSt0, _overrideSt1));

	pShader_->SetMtlParams(paramData_.data());
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

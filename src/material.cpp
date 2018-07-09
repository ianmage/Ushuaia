#include "material.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"


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
		pShader_ = Shader::Load(vsName, fsName);
	}

	itr = reader.FindMember("RenderState");
	if (itr != reader.MemberEnd()) {
		uint8_t const aryBegin = 48;
		uint8_t const aryLast = 126;
		uint8_t const ary = aryLast - aryBegin + 1;
		renderStates = AryStrToNum(itr->value.GetString(), ary, aryBegin);
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

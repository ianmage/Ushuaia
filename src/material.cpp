#include "material.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"


namespace Ushuaia
{

Material::Material(std::string const & _name)
	: name(_name), pShader_(nullptr)
{
}


Material* Material::load(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());

	auto itr = s_mtls.find(k);
	if (itr != s_mtls.end())
		return itr->second;

	Material *pMtl = new Material(_name);
	s_mtls.emplace(k, pMtl);

	pMtl->deserialize();

	return pMtl;
}


void Material::serialize() const
{
	JsonWriter writer;

	if (pShader_) {
		writer.Key("Shader");
		writer.StartArray();
		writer.String(pShader_->vsName());
		writer.String(pShader_->fsName());
		writer.EndArray();
	}

	writer.save("material/" + name);
}


bool Material::deserialize()
{
	JsonReader reader;
	if (!reader.load("material/" + name))
		return false;

	JsonValue::ConstMemberIterator itr;
	itr = reader.FindMember("Shader");
	if (itr != reader.MemberEnd()) {
		std::string vsName = itr->value[0].GetString();
		std::string fsName = itr->value[1].GetString();
		pShader_ = Shader::load(vsName, fsName);
	}

	return true;
}


void Material::pShader(Shader *_pShader)
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


void Material::submit(uint64_t override0, uint64_t override1)
{
	assert(pShader_);
	bgfx::setState(RenderState::overrideMe(renderStates, override0, override1));

	pShader_->setParams(paramData_.data());
}


Vector4* Material::getParamVec4(size_t _nameKey) const
{
	assert(pShader_);
	uint16_t const idx = pShader_->paramIndex(_nameKey);
	if (UINT16_MAX == idx)
		return nullptr;
	auto pData = const_cast<uint8_t*>(&paramData_[idx]);
	return reinterpret_cast<Vector4*>(pData);
}

}

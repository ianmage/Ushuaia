#include "material.h"
#include "../../cpp_common/commUtil.h"


namespace My3D
{

	Material::Material(Shader *_pShader)
		: pShader_(nullptr)
	{
		pShader(_pShader);
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

		for (auto & it : pShader_->paramOffsets())
		{
			Shader::setUniform(it.first, &paramData_[it.second]);
		}
	}


	Vector4* Material::getParamVec4(size_t _nameKey) const
	{
		assert(pShader_);
		auto it = pShader_->paramOffsets().find(_nameKey);
		if (it == pShader_->paramOffsets().end())
			return nullptr;
		auto pData = const_cast<uint8_t*>(&paramData_[it->second]);
		return reinterpret_cast<Vector4*>(pData);
	}

}

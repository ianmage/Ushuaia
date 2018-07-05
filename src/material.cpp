#include "material.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
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

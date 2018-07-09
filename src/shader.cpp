#include "shader.h"
#include "../examples/common/bgfx_utils.h"
#include "../../cpp_common/commUtil.h"
#include "math.h"


namespace Ushuaia
{

	decltype(Shader::s_shaders) Shader::s_shaders;
	decltype(Shader::s_uniforms) Shader::s_uniforms;
	decltype(Shader::s_annos) Shader::s_annos;


	Shader* Shader::Load(std::string const & _vsName, std::string const & _fsName)
	{
		size_t vsID = RT_HASH(_vsName.c_str());
		size_t fsID = RT_HASH(_fsName.c_str());
		size_t k = vsID ^ fsID;
		decltype(s_shaders)::const_iterator itr = s_shaders.find(k);
		if (itr != s_shaders.end())
			return itr->second;
		auto hProgram = loadProgram(_vsName.c_str(), _fsName.c_str());
		if (!isValid(hProgram))
			return nullptr;
		Shader* ret = new Shader(_vsName, _fsName);
		ret->hProgram = hProgram;
		s_shaders[k] = ret;

		ret->Deserailize();

		return ret;
	}


	bgfx::UniformHandle Shader::AddUniform(std::string const & _name,
		bgfx::UniformType::Enum _uType, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		decltype(s_uniforms)::const_iterator itr = s_uniforms.find(k);
		if (itr != s_uniforms.end())
			return itr->second;
		auto ret = bgfx::createUniform(_name.c_str(), _uType, _num);
		s_uniforms[k] = ret;
		return ret;
	}


	bool Shader::Init()
	{
		return s_annos.Load("shaders/annotations");
	}


	void Shader::ClearAll()
	{
		for (auto & m : s_shaders)
		{
			delete m.second;
		}
		s_shaders.clear();
		for (auto & m : s_uniforms)
		{
			bgfx::destroy(m.second);
		}
		s_uniforms.clear();
	}


	Shader::Shader(std::string const & _vsName, std::string const & _fsName)
		: paramSize_(0), vsName_(_vsName), fsName_(_fsName)
	{
	}


	Shader::~Shader()
	{
		bgfx::destroy(hProgram);
	}


	void Shader::Deserailize()
	{
		std::string strKey = vsName_ + "^" + fsName_;
		JsonValue::ConstMemberIterator itr;
		itr = s_annos.FindMember(strKey);
		if (itr != s_annos.MemberEnd()) {
			for (auto & m : itr->value.GetObject()) {
				auto uType = static_cast<bgfx::UniformType::Enum>(m.value[0].GetUint());
				auto uCnt = static_cast<uint16_t>(m.value[1].GetUint());
				AddUniform(m.name.GetString(), uType, uCnt);
			}
		}
	}


	void Shader::addParamInt1(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(int32_t) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			AddUniform(_name, bgfx::UniformType::Int1, _num);
	}

	void Shader::addParamVec4(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(Vector4) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			AddUniform(_name, bgfx::UniformType::Vec4, _num);
	}

	void Shader::addParamMtx3(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(float) * 9 * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			AddUniform(_name, bgfx::UniformType::Mat3, _num);
	}

	void Shader::addParamMtx4(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(Matrix4x4) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			AddUniform(_name, bgfx::UniformType::Mat4, _num);
	}


	uint16_t Shader::ParamIndex(size_t _nameKey) const
	{
		decltype(paramOffsets_)::const_iterator itr = paramOffsets_.find(_nameKey);
		if (itr != paramOffsets_.end())
		{
			return itr->second;
		}
		return UINT16_MAX;
	}


	void Shader::SetParams(uint8_t const * _pData) const
	{
		for (auto & m : paramOffsets_)
		{
			SetUniform(m.first, &_pData[m.second]);
		}
	}


	bool Shader::SetUniform(size_t _nameKey, void const * _pVal, uint16_t _num)
	{
		decltype(s_uniforms)::const_iterator itr = s_uniforms.find(_nameKey);
		if (itr == s_uniforms.end())
			return false;
		bgfx::setUniform(itr->second, _pVal, _num);
		return true;
	}

}

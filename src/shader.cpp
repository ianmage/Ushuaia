#include "shader.h"
#include "../examples/common/bgfx_utils.h"
#include "../../cpp_common/commUtil.h"
#include "../../cpp_common/mathUtil.h"


namespace My3D
{

	decltype(Shader::s_shaders) Shader::s_shaders;
	decltype(Shader::s_uniforms) Shader::s_uniforms;


	Shader* Shader::load(std::string const & _vsName, std::string const & _fsName)
	{
		size_t vsID = RT_HASH(_vsName.c_str());
		size_t fsID = RT_HASH(_fsName.c_str());
		Key k = (vsID << 32) | fsID;
		auto const & it = s_shaders.find(k);
		if (it != s_shaders.end())
			return it->second;
		auto hProgram = loadProgram(_vsName.c_str(), _fsName.c_str());
		if (!isValid(hProgram))
			return nullptr;
		Shader* ret = new Shader(_vsName, _fsName);
		ret->hProgram = hProgram;
		s_shaders[k] = ret;
		return ret;
	}


	bgfx::UniformHandle Shader::addUniform(std::string const & _name,
		bgfx::UniformType::Enum _uType, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		auto it = s_uniforms.find(k);
		if (it != s_uniforms.end())
			return it->second;
		auto ret = bgfx::createUniform(_name.c_str(), _uType, _num);
		s_uniforms[k] = ret;
		return ret;
	}


	void Shader::clearAll()
	{
		for (auto & it : s_shaders)
		{
			delete it.second;
		}
		s_shaders.clear();
		for (auto & it : s_uniforms)
		{
			bgfx::destroy(it.second);
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


	void Shader::addParamInt1(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(int32_t) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			addUniform(_name, bgfx::UniformType::Int1, _num);
	}

	void Shader::addParamVec4(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(Vector4) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			addUniform(_name, bgfx::UniformType::Vec4, _num);
	}

	void Shader::addParamMtx3(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(float) * 9 * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			addUniform(_name, bgfx::UniformType::Mat3, _num);
	}

	void Shader::addParamMtx4(std::string const & _name, uint16_t _num)
	{
		size_t k = RT_HASH(_name.c_str());
		assert(paramOffsets_.find(k) == paramOffsets_.end());
		paramOffsets_[k] = static_cast<uint16_t>(paramSize_);
		paramSize_ += sizeof(Matrix4x4) * _num;
		if (s_uniforms.find(k) == s_uniforms.end())
			addUniform(_name, bgfx::UniformType::Mat4, _num);
	}


	bool Shader::setUniform(size_t _nameKey, void const * _pVal, uint16_t _num)
	{
		auto it = s_uniforms.find(_nameKey);
		if (it == s_uniforms.end())
			return false;
		bgfx::setUniform(it->second, _pVal, _num);
		return true;
	}

}

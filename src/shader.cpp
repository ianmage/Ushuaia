#include "shader.h"
#include "../examples/common/bgfx_utils.h"
#include "../../cpp_common/strUtil.h"
#include "../src/bgfx_p.h"
#ifdef GetObject
#undef GetObject
#endif

#pragma optimize("", off)
#define USE_NEW_PARSE	1


namespace Ushuaia
{

decltype(Shader::s_shaders) Shader::s_shaders;
decltype(Shader::s_shaderHandles) Shader::s_shaderHandles;
decltype(Shader::s_uniforms) Shader::s_uniforms;


Shader* Shader::Load(std::string const & _vsName, std::string const & _fsName)
{
	size_t vsID = RT_HASH(_vsName.c_str());
	size_t fsID = RT_HASH(_fsName.c_str());
	size_t k = vsID ^ fsID;
	decltype(s_shaders)::const_iterator pItr = s_shaders.find(k);
	if (pItr != s_shaders.end())
		return pItr->second;

	decltype(s_shaderHandles)::const_iterator sItr;
	bgfx::ShaderHandle hVtxShader, hFragShader;

	sItr = s_shaderHandles.find(vsID);
	if (sItr != s_shaderHandles.end())
		hVtxShader = sItr->second;
	else {
		hVtxShader = ::loadShader(_vsName.c_str());
		s_shaderHandles[vsID] = hVtxShader;
	}
	sItr = s_shaderHandles.find(fsID);
	if (sItr != s_shaderHandles.end())
		hFragShader = sItr->second;
	else {
		hFragShader = ::loadShader(_fsName.c_str());
		s_shaderHandles[fsID] = hFragShader;
	}

	auto hProgram = bgfx::createProgram(hVtxShader, hFragShader, false);
	if (!isValid(hProgram))
		return nullptr;
	Shader* ret = new Shader(_vsName, _fsName);
	ret->hProgram = hProgram;
	s_shaders[k] = ret;
#if USE_NEW_PARSE
	ret->ParseUniform(hVtxShader);
	ret->ParseUniform(hFragShader);
	ret->uniforms_;
#else
	ret->Deserailize();
#endif

	return ret;
}


bgfx::UniformHandle Shader::AddUniformVec4(std::string const & _name, uint16_t _num)
{
	size_t k = RT_HASH(_name.c_str());
	decltype(s_uniforms)::const_iterator itr = s_uniforms.find(k);
	if (itr != s_uniforms.end())
		return itr->second;

	auto ret = bgfx::createUniform(_name.c_str(),
		bgfx::UniformType::Enum::Vec4, _num);
	s_uniforms[k] = ret;

	return ret;
}


bool Shader::Init()
{
	return true;
}


void Shader::ClearAll()
{
	for (auto & m : s_shaders) {
		delete m.second;
	}
	s_shaders.clear();
	for (auto & m : s_uniforms) {
		bgfx::destroy(m.second);
	}
	s_uniforms.clear();
	for (auto & m : s_shaderHandles) {
		bgfx::destroy(m.second);
	}
}


Shader::Shader(std::string const & _vsName, std::string const & _fsName)
	: paramSize_(0), vsName_(_vsName), fsName_(_fsName)
{
}


Shader::~Shader()
{
	bgfx::destroy(hProgram);
}


std::string Shader::Name() const
{
	std::string vsName = ::strEndsWith(vsName_.c_str(), "_instance") ?
		vsName_.substr(0, vsName_.length() - sizeof("_instance") + 1) : vsName_;
	return vsName + "^" + fsName_;
}


inline uint32_t GetUniformTypeSize(bgfx::UniformType::Enum t)
{
	if (t == bgfx::UniformType::Int1)
		return sizeof(int32_t);
	if (t == bgfx::UniformType::Vec4)
		return sizeof(Vector4);
	if (t == bgfx::UniformType::Mat3)
		return sizeof(Matrix3x3);
	if (t == bgfx::UniformType::Mat4)
		return sizeof(Matrix4x4);
	return 0;
}


void Shader::ParseUniform(bgfx::ShaderHandle hShader)
{
	uint16_t numUniform;
	std::vector<bgfx::UniformHandle> uHandles;
	bgfx::UniformInfo ui;

	numUniform = bgfx::getShaderUniforms(hShader);
	uHandles.resize(numUniform);
	bgfx::getShaderUniforms(hShader, uHandles.data(), numUniform);
	for (auto h : uHandles) {
		char const * uName = bgfx::getName(h);
		if (strStartsWith(uName, "PM_")) {
			uniforms_.emplace(RT_HASH(uName), std::make_pair(h, (uint16_t)paramSize_));
			bgfx::getUniformInfo(h, ui);
			paramSize_ += GetUniformTypeSize(ui.type) * ui.num;
		}
	}
}


Shader* Shader::GetInstance() const
{
	static bool inited = false;
	static Shader* pInst = nullptr;
	if (!inited) {
		inited = true;
		pInst = Load(vsName_ + "_instance", fsName_);
	}
	return pInst;
}


uint16_t Shader::ParamIndex(size_t _nameKey) const
{
#if USE_NEW_PARSE
	decltype(uniforms_)::const_iterator itr = uniforms_.find(_nameKey);
	if (itr != uniforms_.end())
	{
		return itr->second.second;
	}
#else
	decltype(paramOffsets_)::const_iterator itr = paramOffsets_.find(_nameKey);
	if (itr != paramOffsets_.end())
	{
		return itr->second;
	}
#endif
	return UINT16_MAX;
}


void Shader::SetMtlParams(uint8_t const * _pData) const
{
	for (auto & m : uniforms_)
	{
		SetUniform(m.first, &_pData[m.second.second]);
	}
}


void Shader::SaveMtlParams(JsonWriter & _writer, uint8_t const * _pData) const
{
	bgfx::UniformInfo ui;
	for (auto const & m : uniforms_) {
		bgfx::getUniformInfo(m.second.first, ui);
		_writer.Key(ui.name);
		auto pBuf = &_pData[ParamIndex(m.first)];
		if (bgfx::UniformType::Vec4 == ui.type)
			WriteFloatArray(_writer, *(Vector4*)pBuf);
		else if (bgfx::UniformType::Mat3 == ui.type)
			WriteFloatArray(_writer, *(Matrix3x3*)pBuf);
		else if (bgfx::UniformType::Mat4 == ui.type)
			WriteFloatArray(_writer, *(Matrix4x4*)pBuf);
	}
}


bool Shader::SetUniform(size_t _nameKey, void const * _pVal, uint16_t _num) const
{
	decltype(uniforms_)::const_iterator itr = uniforms_.find(_nameKey);
	if (itr == uniforms_.end())
		return false;
	bgfx::setUniform(itr->second.first, _pVal, _num);
	return true;
}

}

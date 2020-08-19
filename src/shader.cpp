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
decltype(Shader::s_handles) Shader::s_handles;


Shader* Shader::Load(std::string const & _vsName, std::string const & _fsName)
{
	size_t vsID = RT_HASH(_vsName.c_str());
	size_t fsID = RT_HASH(_fsName.c_str());
	size_t k = vsID ^ fsID;
	decltype(s_shaders)::const_iterator pItr = s_shaders.find(k);
	if (pItr != s_shaders.end())
		return pItr->second;

	decltype(s_handles)::const_iterator sItr;
	bgfx::ShaderHandle hVtxShader, hFragShader;

	sItr = s_handles.find(vsID);
	if (sItr != s_handles.end())
		hVtxShader = sItr->second;
	else {
		hVtxShader = ::loadShader(_vsName.c_str());
		assert(isValid(hVtxShader));
		s_handles[vsID] = hVtxShader;
	}
	sItr = s_handles.find(fsID);
	if (sItr != s_handles.end())
		hFragShader = sItr->second;
	else {
		hFragShader = ::loadShader(_fsName.c_str());
		assert(isValid(hFragShader));
		s_handles[fsID] = hFragShader;
	}

	auto hTech = bgfx::createProgram(hVtxShader, hFragShader, false);
	if (!isValid(hTech)) {
		assert(false);
		return nullptr;
	}
	Shader* ret = new Shader(_vsName, _fsName);
	ret->hTech = hTech;
	s_shaders[k] = ret;
#if USE_NEW_PARSE
	ret->ParseUniform(hVtxShader);
	ret->ParseUniform(hFragShader);
#else
	ret->Deserailize();
#endif

	return ret;
}


bool Shader::Init()
{
#if 0
	auto hVS = ::loadShader("material/vs_clay");
	assert(isValid(hVS));
	int nuV = bgfx::getShaderUniforms(hVS);
	auto hFS = ::loadShader("material/fs_clay");
	assert(isValid(hFS));
	int nuF = bgfx::getShaderUniforms(hFS);
#endif
	return true;
}


void Shader::ClearAll()
{
	for (auto & m : s_shaders) {
		delete m.second;
	}
	s_shaders.clear();
	for (auto & m : s_handles) {
		bgfx::destroy(m.second);
	}
}


Shader::Shader(std::string const & _vsName, std::string const & _fsName)
	: hTech(BGFX_INVALID_HANDLE), vsName_(_vsName), fsName_(_fsName), paramSize_(0)
{
}


Shader::~Shader()
{
	bgfx::destroy(hTech);
}


std::string Shader::Name() const
{
	std::string vsName = ::StrEndsWith(vsName_.c_str(), "_instance") ?
		vsName_.substr(0, vsName_.length() - sizeof("_instance") + 1) : vsName_;
	return vsName + "^" + fsName_;
}


constexpr uint32_t GetUniformTypeSize(bgfx::UniformType::Enum t)
{
	return (t == bgfx::UniformType::Sampler) ? sizeof(int32_t)
		: (t == bgfx::UniformType::Vec4) ? sizeof(Vector4)
		: (t == bgfx::UniformType::Mat3) ? sizeof(Matrix3x3)
		: (t == bgfx::UniformType::Mat4) ? sizeof(Matrix4x4)
		: 0;
}


constexpr uint8_t PackUniformType(bgfx::UniformType::Enum t)
{
	return (t == bgfx::UniformType::Vec4) ? 1
		: (t == bgfx::UniformType::Mat3) ? 2
		: (t == bgfx::UniformType::Mat4) ? 3
		: 0;
}

constexpr bgfx::UniformType::Enum UnpackUniformType(uint8_t t)
{
	bgfx::UniformType::Enum ut[] = {
		bgfx::UniformType::Sampler,
		bgfx::UniformType::Vec4,
		bgfx::UniformType::Mat3,
		bgfx::UniformType::Mat4
	};
	return ut[t];
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
		if (StrStartsWith(uName, "PM_")) {
			uint16_t offset = static_cast<uint16_t>(paramSize_);
			uniforms_.emplace(RT_HASH(uName), std::make_pair(h, offset));
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


uint16_t Shader::ParamIndex(size_t nameKey) const
{
#if USE_NEW_PARSE
	decltype(uniforms_)::const_iterator itr = uniforms_.find(nameKey);
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


void Shader::SetMtlParams(uint8_t const * pData) const
{
	for (auto & m : uniforms_) {
		bgfx::setUniform(m.second.first, &pData[m.second.second]);
	}
}


void Shader::SaveMtlParams(JsonWriter & writer, uint8_t const * pData) const
{
	bgfx::UniformInfo ui;
	for (auto const & m : uniforms_) {
		bgfx::getUniformInfo(m.second.first, ui);
		writer.Key(ui.name);
		auto pBuf = &pData[ParamIndex(m.first)];
		if (bgfx::UniformType::Vec4 == ui.type)
			WriteFloatArray(writer, (float const*)pBuf, 4 * ui.num);
		else if (bgfx::UniformType::Mat3 == ui.type)
			WriteFloatArray(writer, (float const*)pBuf, 9 * ui.num);
		else if (bgfx::UniformType::Mat4 == ui.type)
			WriteFloatArray(writer, (float const*)pBuf, 16 * ui.num);
	}
	
}


//bool Shader::SetUniform(size_t nameKey, void const * pVal, uint16_t num) const
//{
//	decltype(uniforms_)::const_iterator itr = uniforms_.find(nameKey);
//	if (itr == uniforms_.end())
//		return false;
//	bgfx::setUniform(itr->second.first, pVal, num);
//	return true;
//}

}

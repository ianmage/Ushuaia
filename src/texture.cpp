#include "texture.h"
#include "../../cpp_common/commUtil.h"
#include "../examples/common/bgfx_utils.h"

//#pragma optimize("", off)


namespace Ushuaia
{


Texture::Texture(bgfx::TextureInfo const & texInfo, uint64_t flags, std::string const & name)
: handle_(BGFX_INVALID_HANDLE), info_(texInfo), name_(name)
{
	flags_ = PackTexFlags(flags);
}


Texture::~Texture()
{
	Lost();
}


void Texture::Lost()
{
	if (!managed_ && isValid(handle_))
		bgfx::destroy(handle_);
}


void Texture::Handle(bgfx::TextureHandle hTex, bool managed)
{
	Lost();

	managed_ = managed;
	handle_ = hTex;
}


decltype(TexMgr::s_texs) TexMgr::s_texs;


TexPtr TexMgr::LoadFromFile(std::string const & name, uint32_t samplerFlags, bool isSRGB)
{
	size_t k = RT_HASH(name.c_str());

	auto itr = s_texs.find(k);
	if (itr != s_texs.end()) {
		if (!itr->second.expired())
			return itr->second.lock();
	}

	uint64_t flags = samplerFlags;
	if (isSRGB)
		flags |= BGFX_TEXTURE_SRGB;
	bgfx::TextureInfo texInfo;
	bgfx::TextureHandle hTex = ::loadTexture(("texture/" + name + ".dds").c_str(), flags, (uint8_t)'\000', &texInfo);

	Texture* pTex = new Texture(texInfo, flags, name);
	pTex->Handle(hTex, false);
	std::shared_ptr<Texture> ret(pTex);
	s_texs[k] = ret;
	return ret;
}


TexPtr TexMgr::Get(size_t nameKey)
{
	auto itr = s_texs.find(nameKey);

	if (itr != s_texs.end()) {
		if (itr->second.expired())
			s_texs.erase(nameKey);
		else
			return itr->second.lock();
	}
	return nullptr;
}


void TexMgr::Fini()
{
	for (auto & m : s_texs) {
		auto & tex = m.second;
		if (!tex.expired()) {
			auto pTex = tex.lock();
			bgfx::destroy(pTex->handle_);
			pTex->handle_ = BGFX_INVALID_HANDLE;
		}
	}
	s_texs.clear();
}


decltype(SamplerMgr::s_hSamplers) SamplerMgr::s_hSamplers;


bgfx::UniformHandle SamplerMgr::Get(std::string const & sName)
{
	auto itr = s_hSamplers.find(sName);
	if (itr != s_hSamplers.end())
		return itr->second;

	bgfx::UniformHandle s = bgfx::createUniform(sName.c_str(), bgfx::UniformType::Sampler);
	s_hSamplers[sName] = s;
	return s;
}


void SamplerMgr::Fini()
{
	for (auto & itr : s_hSamplers)
		bgfx::destroy(itr.second);
	s_hSamplers.clear();
}

}

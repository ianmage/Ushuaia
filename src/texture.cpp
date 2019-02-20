#include "texture.h"
#include "../../cpp_common/commUtil.h"
#include "../examples/common/bgfx_utils.h"

#pragma optimize("", off)


namespace Ushuaia
{


Texture::Texture(bgfx::TextureInfo const & texInfo, std::string const & name)
: handle_(BGFX_INVALID_HANDLE), info_(texInfo), name_(name)
{
	stateFlag_ = STATE_NORMAL;
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


std::shared_ptr<Texture> TexMgr::LoadFromFile(std::string const & name)
{
	size_t k = RT_HASH(name.c_str());

	auto itr = s_texs.find(k);
	if (itr != s_texs.end()) {
		if (!itr->second.expired())
			return itr->second.lock();
	}

	bgfx::TextureInfo texInfo;
	bgfx::TextureHandle hTex = ::loadTexture(("texture/" + name + ".dds").c_str(),
		0U, (uint8_t)'\000', &texInfo);

	Texture* pTex = new Texture(texInfo, name);
	pTex->Handle(hTex, false);
	pTex->flag_ = Texture::FROM_FILE;
	if (!isValid(hTex))
		pTex->stateFlag_ = Texture::FILE_NOT_FOUND;
	std::shared_ptr<Texture> ret(pTex);
	s_texs[k] = ret;
	return ret;
}


std::shared_ptr<Texture> TexMgr::Get(size_t nameKey)
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

}

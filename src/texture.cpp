#include "texture.h"
#include "../../cpp_common/commUtil.h"
#include "../examples/common/bgfx_utils.h"

#pragma optimize("", off)


namespace Ushuaia
{

Texture::Texture(std::string const & _name)
	: name_(_name), handle_(BGFX_INVALID_HANDLE)
	, stateFlag_(STATE_NORMAL)
{
	
}


Texture::~Texture()
{
	if (isValid(handle_))
		bgfx::destroy(handle_);
}


bgfx::TextureHandle Texture::Get()
{
	if (isValid(handle_))
		return handle_;
	if (flag_ & FROM_FILE) {
		if (!(stateFlag_ & FILE_NOT_FOUND)) {
			handle_ = loadTexture(("texture/" + name_ + ".dds").c_str());
			if (!isValid(handle_))
				stateFlag_ |= FILE_NOT_FOUND;
		}
	}
	return handle_;
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

	Texture* pTex = new Texture(name);
	pTex->flag_ = Texture::FROM_FILE;
	std::shared_ptr<Texture> ret(pTex);
	s_texs[k] = ret;
	return std::move(ret);
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

}

#include "texture.h"
#include "../../cpp_common/commUtil.h"
#include "../examples/common/bgfx_utils.h"

#pragma optimize("", off)


namespace Ushuaia
{

Texture::Texture(std::string const & _name)
	: name_(_name)
{
	
}


Texture::~Texture()
{
	if (isValid(handle_))
		bgfx::destroy(handle_);
}


bgfx::TextureHandle Texture::Get()
{
	if (!isValid(handle_))
		handle_ = loadTexture(("texture/" + name_ + ".dds").c_str());
	return handle_;
}


decltype(TexMgr::s_texs) TexMgr::s_texs;


Texture* TexMgr::Load(std::string const & name)
{
	size_t k = RT_HASH(name.c_str());

	auto itr = s_texs.find(k);
	if (itr != s_texs.end()) {
		return itr->second;
	}

	Texture *pTex = new Texture(name);
	s_texs[k] = pTex;
	return pTex;
}


Texture* TexMgr::Get(size_t nameKey)
{
	auto itr = s_texs.find(nameKey);
	return (itr != s_texs.end())
		? itr->second : nullptr;
}

}

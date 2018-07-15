#pragma once

#include <string>
#include <unordered_map>
#include <bgfx/bgfx.h>


namespace Ushuaia
{

class TexMgr;


struct Texture
{
public:
	virtual ~Texture();

	bgfx::TextureHandle Get();

private:
	Texture(std::string const & _name);

	std::string name_;
	bgfx::TextureHandle handle_;

	friend class TexMgr;
};


struct TexMgr
{
public:
	static Texture* Load(std::string const & name);
	static Texture* Get(size_t nameKey);

private:
	TexMgr();

	static std::unordered_map<size_t, Texture*> s_texs;
};

}

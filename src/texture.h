#pragma once

#include <string>
#include <map>
#include <memory>
#include <bgfx/bgfx.h>


namespace Ushuaia
{

struct TexMgr;


struct Texture
{
public:
	enum FLAG {
		FROM_FILE = 1,
		FROM_MEM
	};

	virtual ~Texture();

	bgfx::TextureHandle Get();
	std::string const & Name() const { return name_; }

private:
	enum STATE_FLAG {
		STATE_NORMAL,
		FILE_NOT_FOUND
	};

	Texture(std::string const & _name);

	std::string name_;
	bgfx::TextureHandle handle_;
	uint8_t flag_;
	uint8_t stateFlag_;

	friend struct TexMgr;
};


struct TexMgr
{
public:
	static std::shared_ptr<Texture> LoadFromFile(std::string const & name);
	static std::shared_ptr<Texture> Get(size_t nameKey);

private:
	TexMgr();

	static std::map<size_t, std::weak_ptr<Texture>> s_texs;
};

}

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

	Texture(bgfx::TextureInfo const & texInfo, std::string const & name="");
	virtual ~Texture();

	void Lost();

	bgfx::TextureHandle Handle() const { return handle_; }
	void Handle(bgfx::TextureHandle hTex, bool managed);

	bgfx::TextureFormat::Enum Format() const { return info_.format; }
	uint16_t Width() const { return info_.width; }
	uint16_t Height() const { return info_.height; }
	uint16_t Depth() const { return info_.depth; }
	uint16_t NumLayers() const { return info_.numLayers; }
	uint8_t NumMips() const { return info_.numMips; }
	uint8_t BPP() const { return info_.bitsPerPixel; }
	bool IsCube() const { return info_.cubeMap; }
	uint32_t SizeInBytes() const { return info_.storageSize; }

	std::string const & Name() const { return name_; }

private:
	enum STATE_FLAG {
		STATE_NORMAL,
		FILE_NOT_FOUND
	};

	bgfx::TextureHandle handle_;
	bool managed_;
	bgfx::TextureInfo info_;

	std::string name_;
	uint8_t flag_;
	uint8_t stateFlag_;

	friend struct TexMgr;
};


struct TexMgr
{
public:
	static std::shared_ptr<Texture> LoadFromFile(std::string const & name);
	static std::shared_ptr<Texture> Get(size_t nameKey);

	static void Fini();

private:
	TexMgr();

	static std::map<size_t, std::weak_ptr<Texture>> s_texs;
};

}

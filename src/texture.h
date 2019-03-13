#pragma once

#include <string>
#include <map>
#include <memory>
#include <bgfx/bgfx.h>


namespace Ushuaia
{

struct TexMgr;

constexpr uint16_t PackTexFlags(uint64_t flags)
{
	return static_cast<uint16_t>(flags >> BGFX_TEXTURE_RT_MSAA_SHIFT) & 0xfff;
}

constexpr uint64_t UnpackTexFlags(uint16_t flags)
{
	return static_cast<uint64_t>(flags & 0xfff) << BGFX_TEXTURE_RT_MSAA_SHIFT;
}


struct Texture
{
public:
	Texture(bgfx::TextureInfo const & texInfo, uint64_t flags, std::string const & name="");
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
	uint64_t Flags() const { return UnpackTexFlags(flags_); }

	std::string const & Name() const { return name_; }

private:
	bgfx::TextureHandle handle_;
	bool managed_;
	bgfx::TextureInfo info_;
	uint16_t flags_;

	std::string name_;

	friend struct TexMgr;
};

typedef std::shared_ptr<Texture>	TexPtr;


struct TexMgr
{
public:
	static TexPtr LoadFromFile(std::string const & name, uint32_t samplerFlags, bool isSRGB);
	static TexPtr Get(size_t nameKey);

	static void Fini();

private:
	TexMgr();

	static std::map<size_t, std::weak_ptr<Texture>> s_texs;
};


class SamplerMgr
{
public :
	static bgfx::UniformHandle Get(std::string const & sName);

	static void Fini();

private :
	static std::map<std::string, bgfx::UniformHandle> s_hSamplers;
};

}

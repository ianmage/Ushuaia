#pragma once
#include <bgfx/bgfx.h>


namespace Ushuaia
{

class FrameBuffer
{
public:
	FrameBuffer(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt);
	FrameBuffer(uint16_t w, uint16_t h, uint8_t num, bgfx::TextureFormat::Enum const * fmts);
	virtual ~FrameBuffer();

	void Reset();
	void Lost();

	bgfx::FrameBufferHandle Handle() const { return handle_; }
	bgfx::TextureHandle TexHandle(uint8_t attchment) const {
		return bgfx::getTexture(handle_, attchment);
	}

	uint16_t Width() const { return width_; }
	uint16_t Height() const { return height_; }

private:
	bgfx::FrameBufferHandle handle_;

	uint16_t width_, height_;
	uint8_t numRT_;
	bgfx::TextureFormat::Enum fmts_[8];
};

}

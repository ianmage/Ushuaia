#include "frameBuffer.h"
#include <cassert>


namespace Ushuaia
{

FrameBuffer::FrameBuffer(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt)
: width_(w), height_(h), numRT_(1)
, handle_(BGFX_INVALID_HANDLE)
{
	fmts_[0] = fmt;
}


FrameBuffer::FrameBuffer(uint16_t w, uint16_t h, uint8_t num, bgfx::TextureFormat::Enum const * fmts)
: width_(w), height_(h), numRT_(num)
, handle_(BGFX_INVALID_HANDLE)
{
	assert(num > 0 && num <= 8);

	for (uint8_t i = 0; i < num; ++i)
		fmts_[i] = fmts[i];
}


FrameBuffer::~FrameBuffer()
{
	Lost();
}


void FrameBuffer::Reset()
{
	assert(!isValid(handle_));

	uint64_t const samplerFlags = 0
		| BGFX_TEXTURE_RT
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP
		;

	bgfx::TextureHandle rts[8];

	for (uint8_t i = 0; i < numRT_; ++i) {
		rts[i] = bgfx::createTexture2D(width_, height_, false, 1, fmts_[i], samplerFlags);
	}
	handle_ = bgfx::createFrameBuffer(numRT_, rts, true);
}


void FrameBuffer::Lost()
{
	if (isValid(handle_)) {
		bgfx::destroy(handle_);
		handle_ = BGFX_INVALID_HANDLE;
	}
}

}

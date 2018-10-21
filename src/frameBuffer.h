#pragma once
#include <bgfx/bgfx.h>
#include "camera.h"


namespace Ushuaia
{

class FrameBuffer
{
public:
	static bool Init();
	static void Fini();
	static FrameBuffer const * CurrFB() { return s_currFB; }
	static FrameBuffer const * BackBuf() { return s_backBuf; }

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
	uint16_t ViewID() const { return viewID_; }

	void Setup(Camera const *pCam, bgfx::ViewMode::Enum mode, bool doClear) const;

private:
	bgfx::FrameBufferHandle handle_;

	uint16_t width_, height_;
	uint8_t numRT_;
	bgfx::TextureFormat::Enum fmts_[8];

	uint16_t viewID_;

	static uint16_t s_viewCnt;
	static FrameBuffer const * s_currFB;
	static FrameBuffer const * s_backBuf;
};

}

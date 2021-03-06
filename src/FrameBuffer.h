#pragma once
#include <vector>
#include <set>
#include <bgfx/bgfx.h>
#include "Camera.h"
#include "Texture.h"


namespace Ushuaia
{

class FrameBuffer
{
public:
	static bool Init();
	static void Fini();
	static FrameBuffer const * CurrFB() { return s_currFB; }
	static FrameBuffer const * BackBuf() { return s_backBuf; }
	static FrameBuffer const * CheckOut(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt, uint64_t flags=BGFX_TEXTURE_RT, uint8_t mipCnt=1);
	static void CheckIn(FrameBuffer const * &pFB);
	static void Update();
	static uint8_t ViewID() { return s_viewChannelID; }

	FrameBuffer(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt, uint64_t flags=BGFX_TEXTURE_RT, uint8_t mipCnt=1);
	FrameBuffer(bgfx::TextureInfo const * texInfos, uint8_t numRT);
	FrameBuffer(FrameBuffer const & fb) = delete;
	virtual ~FrameBuffer();

	void Reset();
	void Lost();

	bgfx::FrameBufferHandle Handle() const { return handle_; }
	Texture const * pTex(uint8_t attachment = 0) const {
		return &rTexs_[attachment];
	}

	uint16_t Width() const { return width_; }
	uint16_t Height() const { return height_; }
	uint64_t HashCode() const;

	void Setup(Camera const *pCam, bgfx::ViewMode::Enum mode, uint16_t clearFlags,
		uint32_t rgba = 255U, float d = 1.0f, uint8_t s = 0) const;

private:
	bgfx::FrameBufferHandle handle_;

	uint16_t width_, height_;
	std::vector<Texture> rTexs_;

	static FrameBuffer const * s_currFB;
	static FrameBuffer const * s_backBuf;
	static std::set<FrameBuffer const *> s_rts;
	static uint8_t s_viewChannelID;
};

}

#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include <cmath>
#include "math.h"
#include <map>
#include "frameBuffer.h"

//#pragma optimize("", off)


namespace Ushuaia
{

static void MakeScreenQuad(void * pVerts,
	float x, float y, float w, float h)
{
	x = x * 2.f - 1.f;
	y = 1.f - y * 2.f;
	w *= 2.f;
	h *= 2.f;
	float const zz = 0.f;

	TRect<float> xyRect { { x, y-h }, { x+w, y } };

	PosVertex *pVtx = reinterpret_cast<PosVertex*>(pVerts);

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMax.y, zz);
	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	pVtx[2].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	pVtx[3].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
}


void DrawScreenQuad(Shader const *pShader
	, uint64_t state, float x, float y, float w, float h)
{
	assert(4 == bgfx::getAvailTransientVertexBuffer(4, PosVertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 4, PosVertex::s_decl);

	MakeScreenQuad(vb.data, x, y, w, h);

	bgfx::setVertexBuffer(0, &vb);

	state |= BGFX_STATE_PT_TRISTRIP;
	bgfx::setState(state);

	bgfx::submit(FrameBuffer::ViewID(), pShader->Tech());
}


void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView)
{
	Vector2 const ns { near * projWidth, near * projHeight };
	Vector2 const fs { far * projWidth, far * projHeight };

	Vector3 const rawCorners[8] = {
		{ -ns.x,  ns.y, near },
		{  ns.x,  ns.y, near },
		{  ns.x, -ns.y, near },
		{ -ns.x, -ns.y, near },
		{ -fs.x,  fs.y, far },
		{  fs.x,  fs.y, far },
		{  fs.x, -fs.y, far },
		{ -fs.x, -fs.y, far },
	};

	for (uint8_t ii = 0; ii < 8; ++ii) {
		Mul(corners[ii], rawCorners[ii], mtxInvView);
	}
}


/**
* _splits = { near0, far0, near1, far1... nearN, farN }
* N = _numSplits
*/
void SplitFrustum(std::vector<float> & _splits, uint8_t _numSplits
	, float near, float far, float _splitWeight)
{
	float const l = _splitWeight;
	float const ratio = far / near;
	int8_t const numSlices = _numSplits * 2;
	_splits.resize(numSlices);
	float const numSlicesF = static_cast<float>(numSlices);

	_splits[0] = near;

	for (uint8_t nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2) {
		float si = float(ff) / numSlicesF;

		float const nearP = l * (near * bx::pow(ratio, si)) + (1 - l)*(near + (far - near)*si);
		_splits[nn] = nearP;	// near
		_splits[ff] = nearP * 1.005f;	// far from previous split
	}

	// last slice
	_splits[numSlices - 1] = far;
}


void CalcMeshNormal(Vector3 * normals, uint16_t numNorm
	, Vector3 const * vertices, uint16_t numVtx
	, uint16_t const * indices, uint32_t numIdx)
{
	::memset(normals, 0, sizeof(Vector3) * numNorm);

	for (uint32_t i = 0; i < numIdx; i += 3) {
		Vector3 const & v0 = vertices[indices[i]];
		Vector3 const & v1 = vertices[indices[i+1]];
		Vector3 const & v2 = vertices[indices[i+2]];
		Vector3 n = CalcNormal(v0, v1, v2);
		for (uint8_t j = 0; j < 3; ++j) {
			normals[indices[i]] += n;
		}
	}
	
	for (uint16_t i = 0; i < numVtx; ++i) {
		normals[i].Normalize();
	}
}


uint16_t FloatToUint16(float f)
{
	union {
		int16_t i;
		uint16_t ui;
	} v;
	v.i = static_cast<int16_t>(Clamp(f, -1.f, 1.f) * INT16_MAX);
	return v.ui;
}


static Shader *pCopyTech = nullptr;
static Shader *pDown4Tech = nullptr;

static bgfx::UniformHandle hTcOffset = BGFX_INVALID_HANDLE;


void ClearRenderUtilRes()
{
	SafeDelete(pCopyTech);
	SafeDelete(pDown4Tech);
}


void Copy(Texture const *pSrcTex, FrameBuffer const *pOutFB, uint64_t writeState, bool bilinear)
{
	if (pCopyTech == nullptr)
		pCopyTech = Shader::Load("screen/vs_screen_quad", "screen/fs_copy");

	uint32_t samplerFlag = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	if (!bilinear)
		samplerFlag |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;

	pOutFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	SetTexture(0, "src_tex", pSrcTex, samplerFlag);
	bgfx::setState(writeState);
	PostProcess::DrawFullScreen(pCopyTech);
}


void DownQuater(Texture const *pTex, FrameBuffer const *pFB, uint64_t writeState)
{
	if (pDown4Tech == nullptr) {
		pDown4Tech = Shader::Load("screen/vs_down_quater", "screen/fs_down_quater");
		hTcOffset = bgfx::createUniform("uTexCoordOffset", bgfx::UniformType::Vec4, 2);
	}

	uint16_t w = pTex->Width(), h = pTex->Height();
	w = static_cast<uint16_t>(std::max(1, w >> 2));
	h = static_cast<uint16_t>(std::max(1, h >> 2));

	float u = 1.0f / w, v = 1.0f / h;
	Vector2 offsets[4] = {
		{-u, -v}, {+u, -v},
		{+u, +v}, {-u, +v}
	};

	pFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(hTcOffset, offsets, 2);
	uint32_t const linearSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	SetTexture(0, "src_tex", pTex, linearSampler);
	bgfx::setState(writeState);
	PostProcess::DrawFullScreen(pDown4Tech);
}

}

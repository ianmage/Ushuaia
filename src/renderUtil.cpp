#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"


namespace Ushuaia
{

	void screenSpaceQuad(float _texW, float _texH, bool _origBottomLeft, float _w, float _h)
	{
		if (3 == bgfx::getAvailTransientVertexBuffer(3, PosColorTC0Vertex::s_decl))
		{
			bgfx::TransientVertexBuffer vb;
			bgfx::allocTransientVertexBuffer(&vb, 3, PosColorTC0Vertex::s_decl);
			PosColorTC0Vertex *pVert = (PosColorTC0Vertex*)vb.data;

			float const zz = 0.f;

			TRect<float> xyRect{ { -_w, 0.f },{ _w*2.f, _h*2.f } };

			Vector2 const texelHalf{ ViewState::texelHalf.x / _texW,
				ViewState::texelHalf.y / _texH };

			TRect<float> uvRect{ { -1.f + ViewState::texelHalf.x, ViewState::texelHalf.y },{ 2.f, 2.f } };

			if (_origBottomLeft)
			{
				std::swap(uvRect.rMin.y, uvRect.rMax.y);
				uvRect.rMin.y -= 1.f;
				uvRect.rMax.y -= 1.f;
			}

			pVert[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
			pVert[0].rgba = 0xffffffff;
			pVert[0].uv.Set(uvRect.rMin.x, uvRect.rMin.y);

			pVert[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
			pVert[1].rgba = 0xffffffff;
			pVert[1].uv.Set(uvRect.rMax.x, uvRect.rMin.y);

			pVert[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
			pVert[2].rgba = 0xffffffff;
			pVert[2].uv.Set(uvRect.rMax.x, uvRect.rMax.y);

			bgfx::setVertexBuffer(0, &vb);
		}
	}


	void worldSpaceFrustumCorners(FrustumCorners & _corners,
		float _near, float _far, float _projWidth, float _projHeight,
		Matrix4x4 const & _mtxInvView)
	{
		Vector2 const ns{ _near * _projWidth, _near * _projHeight };
		Vector2 const fs{ _far * _projWidth, _far * _projHeight };

		float const corners[8][3] = {
			{ -ns.x,  ns.y, _near },
		{ ns.x,  ns.y, _near },
		{ ns.x, -ns.y, _near },
		{ -ns.x, -ns.y, _near },
		{ -fs.x,  fs.y, _far },
		{ fs.x,  fs.y, _far },
		{ fs.x, -fs.y, _far },
		{ -fs.x, -fs.y, _far },
		};

		for (uint8_t ii = 0; ii < 8; ++ii)
		{
			bx::vec3MulMtx(_corners[ii].v, (float*)&corners[ii], _mtxInvView.v);
		}
	}


	/**
	* _splits = { near0, far0, near1, far1... nearN, farN }
	* N = _numSplits
	*/
	void splitFrustum(std::vector<float> & _splits, uint8_t _numSplits,
		float _near, float _far, float _splitWeight)
	{
		float const l = _splitWeight;
		float const ratio = _far / _near;
		int8_t const numSlices = _numSplits * 2;
		_splits.resize(numSlices);
		float const numSlicesF = static_cast<float>(numSlices);

		_splits[0] = _near;

		for (uint8_t nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2)
		{
			float si = float(ff) / numSlicesF;

			float const nearP = l * (_near * bx::pow(ratio, si)) + (1 - l)*(_near + (_far - _near)*si);
			_splits[nn] = nearP;	// near
			_splits[ff] = nearP * 1.005f;	// far from previous split
		}

		// last slice
		_splits[numSlices - 1] = _far;
	}

}

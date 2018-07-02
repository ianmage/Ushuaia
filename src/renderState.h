#pragma once

#include "bgfx/bgfx.h"
#include "../../cpp_common/mathUtil.h"


namespace My3D
{

	struct RenderState
	{
		enum
		{
			Default = 0,

			//ShadowMap_PackDepth,
			//ShadowMap_PackDepthHoriz,
			//ShadowMap_PackDepthVert,

			Count
		};

		uint64_t state;
		uint32_t blendFactor;
		uint32_t fStencil;
		uint32_t bStencil;

		static uint64_t const s_val[2];

		static uint64_t overrideMe(uint64_t st, uint64_t o0, uint64_t o1) {
			return (st & o0) | o1;
		}
	};

}

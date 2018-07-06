#pragma once

#include "shader.h"
#include <vector>
#include "renderState.h"
#include "serialize.h"


namespace Ushuaia
{

struct Material
{
public:
	struct TexState
	{
		uint32_t flags;
		bgfx::UniformHandle hSampler;
		bgfx::TextureHandle hTex;
		uint8_t stage;
	};

	std::vector<TexState> texStates;

	uint64_t renderStates;

	Material(Shader *_pShader);
	
	void submit(uint64_t override0 = RenderState::s_val[0], uint64_t override1 = RenderState::s_val[1]);
	Vector4* getParamVec4(size_t _nameKey) const;

	Shader* pShader() const { return pShader_; }

	void serialize(Writer& writer) const;

private:
	void pShader(Shader *_pShader);

	Shader* pShader_;

	std::vector<uint8_t> paramData_;
};

}

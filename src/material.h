#pragma once

#include "shader.h"
#include <vector>
#include "renderState.h"


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

	std::string name;

	void submit(uint64_t override0 = RenderState::s_val[0], uint64_t override1 = RenderState::s_val[1]);
	Vector4* getParamVec4(size_t _nameKey) const;

	Shader* pShader() const { return pShader_; }

	void serialize() const;
	bool deserialize();

	static Material* load(std::string const & _name);

private:
	Material(std::string const & _name);

	void pShader(Shader *_pShader);

	Shader* pShader_;

	std::vector<uint8_t> paramData_;

	static std::unordered_map<size_t, Material*> s_mtls;
};

}

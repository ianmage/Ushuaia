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

	void Submit(
		uint64_t _overrideSt0 = RenderState::s_val[0],
		uint64_t _overrideSt1 = RenderState::s_val[1],
		Shader const *_overrideProgram = nullptr);
	Vector4* GetParamVec4(size_t _nameKey) const;

	Shader* GetShader() const { return pShader_; }
	std::string const & Name() const { return name_; }

	void Serialize() const;
	bool Deserialize();

	static Material* Load(std::string const & _name);
	static void ClearAll();

private:
	Material(std::string const & _name);

	void SetShader(Shader *_pShader);

	std::string name_;

	Shader* pShader_;

	std::vector<uint8_t> paramData_;

	static std::unordered_map<size_t, Material*> s_mtls;
};

}

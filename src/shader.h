#pragma once

#include <unordered_map>
#include <map>
#include <bgfx/bgfx.h>
#include "serialize.h"
#include "texture.h"


namespace Ushuaia
{
	struct TexState
	{
		uint32_t flags;
		size_t samplerKey;
		std::shared_ptr<Texture> pTex;
		uint8_t stage;
	};


	struct Shader
	{
	public:
		~Shader();

		bgfx::ProgramHandle Get() const { return hProgram; }

		std::string const & VsName() const { return vsName_; }
		std::string const & FsName() const { return fsName_; }

		std::string Name() const;

		void SetMtlParams(uint8_t const * pData, std::vector<TexState> const & texStates) const;
		void SaveMtlParams(JsonWriter & writer, uint8_t const * pData) const;
		void SaveMtlTexs(JsonWriter & writer, std::vector<TexState> const & texStates) const;

		uint16_t ParamIndex(size_t nameKey) const;
		size_t ParamSize() const { return paramSize_; }

		Shader* GetInstance() const;

		bool SetUniform(size_t nameKey, void const * pVal, uint16_t num = UINT16_MAX) const;

		static bgfx::UniformHandle AddUniformVec4(std::string const & uName, uint16_t _num = 1);

		static Shader* Load(std::string const & _vsName, std::string const & _fsName);

		static bool Init();
		static void ClearAll();

	private:
		Shader(std::string const & _vsName, std::string const & _fsName);

		void ParseUniform(bgfx::ShaderHandle hShader);

		bgfx::ProgramHandle hProgram;
		std::string vsName_, fsName_;

		// PerDraw parameters
#if 0
		std::unordered_map<size_t, uint16_t> paramOffsets_;
#endif
		uint32_t paramSize_;
		typedef std::pair<bgfx::UniformHandle, uint16_t>	ParamInfo;
		std::unordered_map<size_t, ParamInfo> uniforms_;
		std::unordered_map<size_t, bgfx::UniformHandle> samplers_;

		static std::unordered_map<size_t, Shader*> s_shaders;
		static std::unordered_map<size_t, bgfx::ShaderHandle> s_shaderHandles;

		static std::map<size_t, bgfx::UniformHandle> s_uniforms;
	};

}

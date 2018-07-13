#pragma once

#include <bgfx/bgfx.h>
#include <unordered_map>
#include "serialize.h"


namespace Ushuaia
{

	struct Shader
	{
	public:
		bgfx::ProgramHandle hProgram;

		~Shader();

		std::string const & VsName() const { return vsName_; }
		std::string const & FsName() const { return fsName_; }

		std::string Name() const;

		void SetMtlParams(uint8_t const * _pData) const;
		void SaveMtlParams(JsonWriter & _writer, uint8_t const * _pData) const;

		uint16_t ParamIndex(size_t _nameKey) const;
		size_t ParamSize() const { return paramSize_; }

		Shader* GetInstance() const;

		bool SetUniform(size_t _nameKey, void const * _pVal, uint16_t _num = UINT16_MAX) const;

		static bgfx::UniformHandle AddUniformVec4(std::string const & _name, uint16_t _num = 1);

		static Shader* Load(std::string const & _vsName, std::string const & _fsName);

		static bool Init();
		static void ClearAll();

	private:
		Shader(std::string const & _vsName, std::string const & _fsName);

		void ParseUniform(bgfx::ShaderHandle hShader);

		std::string vsName_, fsName_;

		// PerDraw parameters
#if 0
		std::unordered_map<size_t, uint16_t> paramOffsets_;
#endif
		uint32_t paramSize_;
		typedef std::pair<bgfx::UniformHandle, uint16_t>	ParamInfo;
		std::unordered_map<size_t, ParamInfo> uniforms_;

		static std::unordered_map<size_t, Shader*> s_shaders;
		static std::unordered_map<size_t, bgfx::ShaderHandle> s_shaderHandles;

		static std::unordered_map<size_t, bgfx::UniformHandle> s_uniforms;
	};

}

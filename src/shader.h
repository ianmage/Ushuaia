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
		size_t paramSize_;

		~Shader();

		void Deserailize();

		uint16_t ParamIndex(size_t _nameKey) const;
		void SetParams(uint8_t const * _pData) const;

		static std::unordered_map<size_t, std::string> s_vec4NameMap;

		static bgfx::UniformHandle AddUniformVec4(std::string const & _name, uint16_t _num = 1);
		static bool SetUniform(size_t _nameKey, void const * _pVal, uint16_t _num = UINT16_MAX);

		static Shader* Load(std::string const & _vsName, std::string const & _fsName);

		static bool Init();
		static void ClearAll();

	private:
		Shader(std::string const & _vsName, std::string const & _fsName);

		std::string vsName_, fsName_;

		std::unordered_map<size_t, uint16_t> paramOffsets_;

		static std::unordered_map<size_t, Shader*> s_shaders;

		static std::unordered_map<size_t, bgfx::UniformHandle> s_uniforms;

		static JsonReader s_annos;

	public:
		std::string const & VsName() const {
			return vsName_;
		}
		std::string const & FsName() const {
			return fsName_;
		}
	};

}

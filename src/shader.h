#pragma once

#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>
#include "math.h"


namespace Ushuaia
{

	struct Shader
	{
	public:
		typedef uint64_t	Key;

		bgfx::ProgramHandle hProgram;
		size_t paramSize_;

		~Shader();

		void addParamInt1(std::string const & _name, uint16_t _num = 1);
		void addParamVec4(std::string const & _name, uint16_t _num = 1);
		void addParamMtx3(std::string const & _name, uint16_t _num = 1);
		void addParamMtx4(std::string const & _name, uint16_t _num = 1);

		uint16_t paramIndex(size_t _nameKey) const;
		void setParams(uint8_t const * _pData) const;

		static bgfx::UniformHandle addUniform(std::string const & _name,
			bgfx::UniformType::Enum _uType, uint16_t _num = 1);
		static bool setUniform(size_t _nameKey, void const * _pVal, uint16_t _num = UINT16_MAX);

		static Shader* load(std::string const & _vsName, std::string const & _fsName);

		static void clearAll();

	private:
		Shader(std::string const & _vsName, std::string const & _fsName);

		std::string vsName_, fsName_;

		std::unordered_map<size_t, uint16_t> paramOffsets_;

		static std::unordered_map<Key, Shader*> s_shaders;

		static std::unordered_map<size_t, bgfx::UniformHandle> s_uniforms;

	public:
		std::string const & vsName() const {
			return vsName_;
		}
		std::string const & fsName() const {
			return fsName_;
		}
	};

}

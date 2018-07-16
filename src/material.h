#pragma once

#include "shader.h"
#include "renderState.h"


namespace Ushuaia
{

struct Material
{
public:
	std::vector<TexState> texStates;

	uint64_t renderState;

	float* GetParam(size_t nameKey) const;
	inline bool SetParamVec4(size_t nameKey, float _x, float _y, float _z, float _w) {
		auto v4 = reinterpret_cast<Vector4*>(GetParam(nameKey));
		if (!v4) return false;
		v4->x = _x; v4->y = _y; v4->z = _z; v4->w = _w;
		return true;
	}

	void SubmitParams(Shader const * pOverrideShader = nullptr) const {
		Shader const * pShader = pOverrideShader ? pOverrideShader : pShader_;
		pShader->SetMtlParams(paramData_.data(), texStates);
	}

	Shader* GetShader() const { return pShader_; }
	void SetShader(Shader *_pShader);
	//std::string const & Name() const { return name_; }

	void Serialize() const;
	bool Deserialize();
	void Serialize(JsonWriter & writer) const;
	bool Deserialize(JsonValue const & jsObj);

	//static Material* Load(std::string const & _name);
	//static void ClearAll();

private:
	//Material(std::string const & _name);

	//std::string name_;

	Shader* pShader_;

	std::vector<uint8_t> paramData_;

	//static std::unordered_map<size_t, Material*> s_mtls;
};

}

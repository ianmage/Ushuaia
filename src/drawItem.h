#pragma once

#include "material.h"
#include "mesh.h"


namespace Ushuaia
{

struct DrawUnit
{
	PrimitiveGroup const * primGroup;
	Material const * pMtl;

	void Submit(uint64_t overrideSt0, uint64_t overrideSt1) const;
};


struct DrawItem : public DrawUnit
{
	Matrix4x4 transform;

	bool isValid;

	void Submit(uint64_t overrideSt0, uint64_t overrideSt1) const;
};


struct InstanceItem : public DrawUnit
{
	std::vector<Matrix4x4> transforms;

	Shader * pInstShader;

	DrawItem const & operator=(DrawItem const & di) {
		primGroup = di.primGroup;
		pMtl = di.pMtl;
		pInstShader = di.pMtl->GetShader()->GetInstance();
		return di;
	}

	void Submit(uint64_t overrideSt0, uint64_t overrideSt1) const;
};

struct DrawChannel
{
public:
	static bool s_supportInstancing;

	static void Init();
	static void Gather();
	static DrawItem& Add();
	static void DrawOpaque(uint64_t override0, uint64_t override1);
	static void ClearAll();

private:
	static std::vector<DrawItem> s_opaque;
	static std::vector<InstanceItem> s_instance;
};

}

#pragma once
#include "Math.h"


namespace Ushuaia
{

struct Camera
{
	float fovY;
	float aspect;
	float near;
	float far;

	Matrix4x4 mtxView;
	Matrix4x4 mtxProj;
	Matrix4x4 mtxInvView;

	Vector4 frustum[6];	// near, far, left, right, top, bottom

	Camera();
	virtual ~Camera();

	void Init(float _fovY, float _aspect, float _near, float _far);
	void RecalcProj();
	void SetPos(Vector3 const & pos);
	void SetHorizontalAngle(float ha);
	void SetVerticalAngle(float va);

	void Update();

	bool IsVisible(Vector4 const & boundSphere);

	Vector3 ViewExpansionVector() const;
	void WorldExpansionBasis(Vector4 & vBasisX, Vector4 & vBasisY, Vector4 & vBasisZ) const;

	static Camera* pCurrent;

	static bool InitDefault();
	static bool FiniDefault();
};

}

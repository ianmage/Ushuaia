#pragma once
#include "math.h"


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
		Matrix4x4 mtxInvProj;

		Camera();
		virtual ~Camera();

		void Init(float _fovY, float _aspect, float _near, float _far);
		void RecalcProj();
		void SetPos(Vector3 const & pos);
		void SetHorizontalAngle(float ha);
		void SetVerticalAngle(float va);

		void Update();

		static Camera* pCurrent;

		static bool InitDefault();
		static bool FiniDefault();
	};

}

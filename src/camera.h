#pragma once
#include "../../cpp_common/mathUtil.h"


namespace My3D
{

	struct Camera
	{
		float fovY;
		float aspect;
		float near;
		float far;

		Matrix4x4 mtxView;
		Matrix4x4 mtxProj;

		Camera();
		virtual ~Camera();

		void init(float _fovY, float _aspect, float _near, float _far);
		void recalcProj();
		void setPos(Vector3 const & pos);
		void setHorizontalAngle(float ha);
		void setVerticalAngle(float va);

		void update();

		static Camera* pCurrent;

		static bool initDefault();
		static bool finiDefault();
	};

}

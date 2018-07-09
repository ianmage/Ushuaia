#include "camera.h"
#include "bgfx/bgfx.h"
#include "../examples/common/camera.h"
#include "bx/math.h"
#include "viewState.h"
#include "appConf.h"


namespace Ushuaia
{

	Camera* Camera::pCurrent = nullptr;


	bool Camera::InitDefault()
	{
		if (pCurrent)
			return false;
		pCurrent = new Camera;
		return true;
	}

	bool Camera::FiniDefault()
	{
		if (!pCurrent)
			return false;
		delete pCurrent;
		pCurrent = nullptr;
		return true;
	}


	Camera::Camera()
	{
		cameraCreate();
	}


	Camera::~Camera()
	{
		cameraDestroy();
	}


	void Camera::Init(float _fovY, float _aspect, float _near, float _far)
	{
		fovY = _fovY;
		aspect = _aspect;
		near = _near;
		far = _far;

		RecalcProj();
	}


	void Camera::RecalcProj()
	{
		bgfx::Caps const * caps = bgfx::getCaps();
		bx::mtxProj(mtxProj.v, fovY, aspect, near, far, caps->homogeneousDepth);
	}


	void Camera::SetPos(Vector3 const & pos)
	{
		cameraSetPosition(pos.v);
	}

	void Camera::SetHorizontalAngle(float ha)
	{
		cameraSetHorizontalAngle(ha);
	}

	void Camera::SetVerticalAngle(float va)
	{
		cameraSetVerticalAngle(va);
	}


	void Camera::Update()
	{
		cameraUpdate(AppConf::deltaTime, AppConf::mouseState);
		cameraGetViewMtx(mtxView.v);
	}

}

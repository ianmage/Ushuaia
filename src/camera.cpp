#include "camera.h"
#include "bgfx/bgfx.h"
#include "../examples/common/camera.h"
#include "bx/math.h"
#include "viewState.h"
#include "appConf.h"


namespace My3D
{

	Camera* Camera::pCurrent = nullptr;


	bool Camera::initDefault()
	{
		if (pCurrent)
			return false;
		pCurrent = new Camera;
		return true;
	}

	bool Camera::finiDefault()
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


	void Camera::init(float _fovY, float _aspect, float _near, float _far)
	{
		fovY = _fovY;
		aspect = _aspect;
		near = _near;
		far = _far;

		recalcProj();
	}


	void Camera::recalcProj()
	{
		bgfx::Caps const * caps = bgfx::getCaps();
		bx::mtxProj(mtxProj.v, fovY, aspect, near, far, caps->homogeneousDepth);
	}


	void Camera::setPos(Vector3 const & pos)
	{
		cameraSetPosition(pos.v);
	}

	void Camera::setHorizontalAngle(float ha)
	{
		cameraSetHorizontalAngle(ha);
	}

	void Camera::setVerticalAngle(float va)
	{
		cameraSetVerticalAngle(va);
	}


	void Camera::update()
	{
		cameraUpdate(AppConf::deltaTime, AppConf::mouseState);
		cameraGetViewMtx(mtxView.v);
	}

}

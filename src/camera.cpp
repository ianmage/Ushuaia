#include "camera.h"
#include "bgfx/bgfx.h"
#include "../examples/common/camera.h"
#include "bx/math.h"
#include "viewState.h"
#include "appConf.h"
#include "../examples/common/bounds.h"
#ifdef _APPLE
#include <tgmath.h>
#endif


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

	bx::mtxInverse(mtxInvView.v, mtxView.v);

	Matrix4x4 mtxViewProj;
	MtxMultiply(mtxViewProj, mtxView, mtxProj);

	::Plane *planes = reinterpret_cast<::Plane*>(frustum);
	buildFrustumPlanes(planes, mtxViewProj.v);
}


bool Camera::IsVisible(Vector4 const & boundSphere)
{
	bool ret = true;
	Vector3 const & center = boundSphere.Vec3();
	float radius = boundSphere.w;
	for (uint8_t i = 0; i < 6; ++i) {
		Vector4 const & plane = frustum[i];
		float dist = center.Dot(plane.Vec3()) + plane.w;
		if (dist < -radius) {	// depend on plane normal, otherwise < -radius
			ret = false;
			break;
		}
	}
	return ret;
}


Vector3 Camera::ViewExpansionVector() const
{	// could be property
	float D = far * ::tanf(fovY * 0.5f / 180.f * (float)MATH_PI);
	Vector3 corner { D * aspect, D, far };
	corner /= far;
	return corner;
}

}

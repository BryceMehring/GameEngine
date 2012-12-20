
#include "Camera.h"
#include <dinput.h>

Camera* CreateCamera()
{
	return new Camera();
}
void ReleaseCamera(Camera* pCam)
{
	if(pCam != nullptr)
	{
		pCam->Release();
	}
}

Camera::Camera() : m_width(0.0f), m_height(0.0f)
{
	memset(&m_PosW,0,sizeof(D3DXVECTOR3));
	memset(&m_LookW,0,sizeof(D3DXVECTOR3));
	memset(&m_RightW,0,sizeof(D3DXVECTOR3));
	memset(&m_dir,0,sizeof(D3DXVECTOR3));

	m_UpW.x = m_UpW.z = 0.0f;
	m_UpW.y = 1.0f;
}

float Camera::GetWidth() const
{
	return m_width;
}
float Camera::GetHeight() const
{
	return m_height;
}

const D3DXMATRIX& Camera::view() const
{
	return m_View;
}
const D3DXMATRIX& Camera::proj() const
{
	return m_Proj;
}
const D3DXMATRIX& Camera::viewProj() const
{
	return m_ViewProj;
}

D3DXVECTOR3& Camera::pos()
{
	return m_PosW;
}

void Camera::lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
	m_PosW = pos;
	m_LookW = target;
	m_UpW = up;

	buildView();
}

void Camera::setLens(float w, float h, float nearZ, float farZ)
{
	m_width = w;
	m_height = h;
	D3DXMatrixOrthoLH(&m_Proj,w,h,nearZ,farZ);
	//D3DXMatrixOrthoLH
	//D3DXMatrixPerspectiveFovLH(&m_Proj,fov,aspect,nearZ,farZ);
}

void Camera::setDir(const D3DXVECTOR3& dir)
{
	//m_dir = dir;
}

void Camera::update(float dt)
{
	//m_PosW += m_dir * dt;

	/*float pitch = input.MouseY() / 150.0f;
	float yAngle = input.MouseX() / 150.0f;

	D3DXMATRIX R;

	D3DXMatrixRotationAxis(&R,&m_RightW,pitch);
	D3DXVec3TransformCoord(&m_LookW,&m_LookW,&R);
	D3DXVec3TransformCoord(&m_UpW,&m_UpW,&R);

	D3DXMatrixRotationY(&R,yAngle);
	D3DXVec3TransformCoord(&m_RightW,&m_RightW,&R);
	D3DXVec3TransformCoord(&m_UpW,&m_UpW,&R);
	D3DXVec3TransformCoord(&m_LookW,&m_LookW,&R);*/
	//::D3DXMatrixLookAtLH(&m_View,&m_PosW,


	//D3DXMatrixLookAtLH(&m_View,&m_PosW,&m_LookW,&m_UpW);

	//buildView();

	m_ViewProj = m_View * m_Proj;

}

void Camera::buildView()
{
	//::D3DXMatrixScaling(&m_View,.5f,.5f,.5f);
	D3DXMatrixLookAtLH(&m_View,&m_PosW,&m_LookW,&m_UpW);

	/*D3DXVec3Normalize(&m_LookW,&m_LookW);
	D3DXVec3Cross(&m_UpW,&m_LookW,&m_RightW);
	D3DXVec3Normalize(&m_UpW,&m_UpW);

	D3DXVec3Cross(&m_RightW,&m_UpW,&m_LookW);
	D3DXVec3Normalize(&m_RightW,&m_RightW);

	float x = -D3DXVec3Dot(&m_PosW,&m_RightW);
	float y = -D3DXVec3Dot(&m_PosW,&m_UpW);
	float z = -D3DXVec3Dot(&m_PosW,&m_LookW);

	m_View(0,0) = m_RightW.x;
	m_View(1,0) = m_RightW.y;
	m_View(2,0) = m_RightW.z;
	m_View(3,0) = x;

	m_View(0,1) = m_UpW.x;
	m_View(1,1) = m_UpW.y;
	m_View(2,1) = m_UpW.z;
	m_View(3,1) = y;

	m_View(0,2) = m_LookW.x;
	m_View(1,2) = m_LookW.y;
	m_View(2,2) = m_LookW.z;
	m_View(3,2) = z;

	m_View(0,3) = 0.0f;
	m_View(1,3) = 0.0f;
	m_View(2,3) = 0.0f;
	m_View(3,3) = 1.0f;*/
}
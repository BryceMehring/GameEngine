
#include "Camera.h"
#include <dinput.h>

Camera::Camera() : m_Speed(1.0f)
{
	memset(&m_PosW,0,sizeof(D3DXVECTOR3));
	memset(&m_LookW,0,sizeof(D3DXVECTOR3));
	memset(&m_RightW,0,sizeof(D3DXVECTOR3));

	m_UpW.x = m_UpW.z = 0.0f;
	m_UpW.y = 1.0f;
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

void Camera::setLens(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&m_Proj,fov,aspect,nearZ,farZ);
}

void Camera::setSpeed(float s)
{
	m_Speed = s;
}

void Camera::update(IKMInput& input, float dt)
{
	D3DXVECTOR3 dir(0.0f,0.0f,0.0f);
	if(input.KeyDown(DIK_W))
	{
		dir += m_LookW;
	}
	if(input.KeyDown(DIK_S))
	{
		dir -= m_LookW;
	}
	if(input.KeyDown(DIK_D))
	{
		dir += m_RightW;
	}
	if(input.KeyDown(DIK_D))
	{
		dir -= m_RightW;
	}

	D3DXVec3Normalize(&dir,&dir);
	m_PosW += dir*m_Speed*dt;

	float pitch = input.MouseY() / 150.0f;
	float yAngle = input.MouseX() / 150.0f;

	D3DXMATRIX R;

	D3DXMatrixRotationAxis(&R,&m_RightW,pitch);
	D3DXVec3TransformCoord(&m_LookW,&m_LookW,&R);
	D3DXVec3TransformCoord(&m_UpW,&m_UpW,&R);

	D3DXMatrixRotationY(&R,yAngle);
	D3DXVec3TransformCoord(&m_RightW,&m_RightW,&R);
	D3DXVec3TransformCoord(&m_UpW,&m_UpW,&R);
	D3DXVec3TransformCoord(&m_LookW,&m_LookW,&R);

	buildView();

	m_ViewProj = m_View * m_Proj;

}

void RegisterScript(asVM* pVM)
{
	//asIScriptEngine& engine = pVM->GetScriptEngine();

///	DBAS(engine.RegisterObjectType("Camera",0,asOBJ_REF));
//	DBAS(engine.RegisterObjectMethod("Camera","void setLens(float, float, float, float)",asMETHOD(Camera,setLens),asCALL_THISCALL));
//	DBAS(engine.RegisterObjectMethod("Camera","void setSpeed(float)",asMETHOD(Camera,setSpeed),asCALL_THISCALL));

	//RegisterRefCountingObject(Camera,engine);
}

void Camera::buildView()
{
	D3DXVec3Normalize(&m_LookW,&m_LookW);
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
	m_View(3,3) = 1.0f;
}
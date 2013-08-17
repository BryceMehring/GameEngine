
#include "Camera.h"
#include "glm/gtx/transform.hpp"

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

Camera::Camera() : m_View(1.0f), m_Proj(1.0f),
	m_ViewProj(1.0f), m_PosW(0.0f), m_RightW(0.0f),
	m_UpW(0.0f), m_LookW(0.0f),
	m_width(0.0f), m_height(0.0f)
{
}

float Camera::GetWidth() const
{
	return m_width;
}
float Camera::GetHeight() const
{
	return m_height;
}

const glm::mat4& Camera::view() const
{
	return m_View;
}
const glm::mat4& Camera::proj() const
{
	return m_Proj;
}
const glm::mat4& Camera::viewProj() const
{
	return m_ViewProj;
}

glm::vec3& Camera::pos()
{
	return m_PosW;
}

void Camera::lookAt(const glm::vec3& pos,const glm::vec3& target,const glm::vec3& up)
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
	m_Proj = glm::ortho(-m_width/2,m_width/2,-m_height/2,m_height/2,nearZ,farZ);
	//m_Proj = glm::perspective<float>(150.0f,1920.0f/1080.0f,nearZ,farZ);
	//m_Proj =  glm::infinitePerspective(90.0f,1920.0f/1080.0f,10.0f);
}

void Camera::setDir(const glm::vec3& dir)
{
	//m_dir = dir;
}

void Camera::update(float dt)
{
	m_ViewProj = m_Proj * m_View;

}

void Camera::buildView()
{
	m_View = glm::lookAt(m_PosW,m_LookW,m_UpW);

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

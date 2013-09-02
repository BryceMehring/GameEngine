
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

Camera::Camera() : m_View(1.0f), m_Proj(1.0f), m_ViewProj(1.0f), m_PosW(0.0f), m_UpW(0.0f), m_LookW(0.0f), m_width(0.0f), m_height(0.0f)
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
}

void Camera::setLens(float fov, float w, float h, float nearZ, float farZ)
{
	m_width = w;
	m_height = h;
	m_Proj =  glm::perspective(fov,w/h,nearZ,farZ); // 90.0f,1920.0f/1080.0f,1.0f,2000.0f
}

void Camera::update()
{
	m_ViewProj = m_Proj * m_View;
}

void Camera::buildView()
{
	m_View = glm::lookAt(m_PosW,m_LookW,m_UpW);
}

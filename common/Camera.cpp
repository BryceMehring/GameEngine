
#include "Camera.h"
#include "glm/gtx/transform.hpp"

Camera::Camera() : m_View(1.0f), m_Proj(1.0f), m_ViewProj(1.0f)
{
}

const glm::mat4& Camera::View() const
{
	return m_View;
}
const glm::mat4& Camera::Proj() const
{
	return m_Proj;
}
const glm::mat4& Camera::ViewProj() const
{
	return m_ViewProj;
}

bool Camera::IsVisible(const glm::vec3& min, const glm::vec3& max) const
{
	glm::vec3 P;
	glm::vec3 Q;

	for(unsigned int i = 0; i < 6; ++i)
	{
		for(unsigned int j = 0; j < 3; ++j)
		{
			if(m_planes[i][j] >= 0.0f)
			{
				P[j] = min[j];
				Q[j] = max[j];
			}
			else
			{
				P[j] = max[j];
				Q[j] = min[j];
			}
		}

		if(glm::dot(m_planes[i],glm::vec4(Q.x,Q.y,Q.z,1.0f)) < 0.0f)
			return false;

	}

	return true;
}

void Camera::LookAt(const glm::vec3& pos,const glm::vec3& target,const glm::vec3& up)
{
	m_View = glm::lookAt(pos, target, up);
}

void Camera::SetLens(float w, float h, float nearZ, float farZ)
{
	m_Proj = glm::ortho(0.0f,w,0.0f,h,nearZ,farZ); // left, right, bottom, top
}

void Camera::SetLens(float fov, float w, float h, float nearZ, float farZ)
{
	m_Proj = glm::perspective(fov,w/h,nearZ,farZ);
}

void Camera::Update()
{
	m_ViewProj = m_Proj * m_View;
	BuildFrustumPlanes();
}

void Camera::BuildFrustumPlanes()
{
	glm::vec4 col0(m_ViewProj[0][0],m_ViewProj[1][0],m_ViewProj[2][0],m_ViewProj[3][0]);
	glm::vec4 col1(m_ViewProj[0][1],m_ViewProj[1][1],m_ViewProj[2][1],m_ViewProj[3][1]);
	glm::vec4 col2(m_ViewProj[0][2],m_ViewProj[1][2],m_ViewProj[2][2],m_ViewProj[3][2]);
	glm::vec4 col3(m_ViewProj[0][3],m_ViewProj[1][3],m_ViewProj[2][3],m_ViewProj[3][3]);

	m_planes[0] = col2;
	m_planes[1] = col3 - col2;
	m_planes[2] = col3 + col0;
	m_planes[3] = col3 - col0;
	m_planes[4] = col3 - col1;
	m_planes[5] = col3 + col1;

	// Normalize the planes
	for(unsigned int i = 0; i < 6; ++i)
	{
		float length = glm::length(glm::vec3(m_planes[i].x,m_planes[i].y,m_planes[i].z));
		m_planes[i] /= length;
	}

}

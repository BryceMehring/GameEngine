
#include "Camera.h"
#include <glm/gtx/transform.hpp>

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
	glm::vec3 Q;

	for(unsigned int i = 0; i < 6; ++i)
	{
		for(unsigned int j = 0; j < 3; ++j)
		{
			if(m_planes[i][j] >= 0.0f)
			{
				Q[j] = max[j];
			}
			else
			{
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

void Camera::SetLens(float fov, float w, float h, float nearZ, float farZ)
{
	m_Proj = glm::ortho(0.0f, w, 0.0f, h, nearZ, farZ); // left, right, bottom, top
}

void Camera::UpdateAspectRatio(float w, float h)
{
	m_Proj[0][0] = 2.0f / w;
	m_Proj[1][1] = 2.0f / h;
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

void PerspectiveCamera::SetLens(float fov, float w, float h, float nearZ, float farZ)
{
	m_fov = fov;
	m_Proj = glm::perspective(fov, w / h, nearZ, farZ);
}

void PerspectiveCamera::UpdateAspectRatio(float w, float h)
{
	float tanHalfFovy = tan(m_fov / float(2));

	m_Proj[0][0] = float(1) / ((w / h) * tanHalfFovy);
}

glm::vec3 PerspectiveCamera::UnProjectSS(const glm::vec3& pos, const glm::vec4& viewport) const
{
	return glm::unProject(pos, m_View, m_Proj, viewport);
}

glm::vec3 PerspectiveCamera::UnProjectWS(glm::vec3 pos, const glm::vec4& viewport) const
{
	pos.z = glm::project(pos, m_View, m_Proj, viewport).z;
	return UnProjectSS(pos, viewport);
}

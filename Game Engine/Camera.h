#ifndef _CAMERA_
#define _CAMERA_

#include "RefCounting.h"
#include <glm\glm.hpp>

class Camera : public RefCounting
{
public:

	const glm::mat4& view() const;
	const glm::mat4& proj() const;
	const glm::mat4& viewProj() const;

	const glm::mat4& right() const;
	const glm::mat4& up() const;
	const glm::mat4& look() const;

	float GetWidth() const;
	float GetHeight() const;

	glm::vec3& pos();

	void lookAt(glm::vec3& pos, glm::vec3& target, glm::vec3& up);

	void setLens(float w, float h, float nearZ, float farZ);

	void setDir(const glm::vec3& dir);

	void update(float dt);

protected:

	friend Camera* CreateCamera();
	friend void ReleaseCamera(Camera*); 

	Camera();
	virtual ~Camera() {}

	void buildView();

protected:

	glm::mat4 m_View;
	glm::mat4 m_Proj;
	glm::mat4 m_ViewProj;

	glm::vec3 m_PosW;
	glm::vec3 m_RightW;
	glm::vec3 m_UpW;
	glm::vec3 m_LookW;

	glm::vec3 m_dir;

	float m_width;
	float m_height;

	Camera(const Camera&);
	Camera& operator=(const Camera&);

};

Camera* CreateCamera();
void ReleaseCamera(Camera*);

#endif // _CAMERA_
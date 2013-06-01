#ifndef _CAMERA_
#define _CAMERA_

#include "RefCounting.h"
#include <glm/glm.hpp>

// Manages the matrices need to create a camera
class Camera : public RefCounting
{
public:

	const glm::mat4& view() const; // returns the view matrix
	const glm::mat4& proj() const; // // returns the projection matrix
	const glm::mat4& viewProj() const; // returns the viewProj matrix (proj * view)

	const glm::mat4& look() const; // returns the view matrix

	float GetWidth() const; // Width of the 2D field view
	float GetHeight() const; // Height of the 2D field view

	glm::vec3& pos(); // Position of the camera

	// creates the view matrix
	// pos = pos of camera
	// target = where the camera should look
	// up = which direction is up
	void lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

	// Creates a 2D ortho matrix
	// w = width of the 2D field
	// h = height of the 2D field
	// nearZ = near clip plane
	// farZ = far clip plane
	void setLens(float w, float h, float nearZ, float farZ);

	// todo: these are not used, remove them
	void setDir(const glm::vec3& dir);
	void update(float dt);

protected:

	friend Camera* CreateCamera();
	friend void ReleaseCamera(Camera*);

	Camera();
	virtual ~Camera() {}

	void buildView();

protected:

	glm::mat4 m_View; // View matrix
	glm::mat4 m_Proj; // Projection matrix
	glm::mat4 m_ViewProj; // Projection * View

	glm::vec3 m_PosW; // World Pos
	glm::vec3 m_RightW; // Right vector
	glm::vec3 m_UpW; // up vector
	glm::vec3 m_LookW; // target vector

	float m_width; // Width of the 2D field view
	float m_height; // Height of the 2D field view

	// no copying
	Camera(const Camera&);
	Camera& operator=(const Camera&);

};

// used to create and release cameras
Camera* CreateCamera();
void ReleaseCamera(Camera*);

#endif // _CAMERA_

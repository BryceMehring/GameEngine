#ifndef _CAMERA_
#define _CAMERA_

#include <glm/glm.hpp>
#include "RefCounting.h"

// Manages the matrices need to create a camera
class Camera : public RefCounting
{
public:

	COMMON_API const glm::mat4& view() const; // returns the view matrix
	COMMON_API const glm::mat4& proj() const; // // returns the projection matrix
	COMMON_API const glm::mat4& viewProj() const; // returns the viewProj matrix (proj * view)

	COMMON_API const glm::mat4& look() const; // returns the view matrix

	COMMON_API float GetWidth() const; // Width of the 2D field view
	COMMON_API float GetHeight() const; // Height of the 2D field view

	COMMON_API glm::vec3& pos(); // Position of the camera

	COMMON_API bool IsVisible(const glm::vec3& min, const glm::vec3& max) const;

	// creates the view matrix
	// pos = pos of camera
	// target = where the camera should look
	// up = which direction is up
	COMMON_API void lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

	// Creates a 2D ortho matrix
	// w = width of the 2D field
	// h = height of the 2D field
	// nearZ = near clip plane
	// farZ = far clip plane
	COMMON_API void setLens(float w, float h, float nearZ, float farZ);
	COMMON_API void setLens(float fov, float w, float h, float nearZ, float farZ);

	COMMON_API void update();

private:

	Camera();
	virtual ~Camera() {}

	void buildView();

private:

	glm::mat4 m_View; // View matrix
	glm::mat4 m_Proj; // Projection matrix
	glm::mat4 m_ViewProj; // Projection * View

	glm::vec3 m_PosW; // World Pos
	glm::vec3 m_UpW; // up vector
	glm::vec3 m_LookW; // target vector

	glm::vec4 m_planes[6];

	float m_width; // Width of the 2D field view
	float m_height; // Height of the 2D field view

	friend COMMON_API Camera* CreateCamera();
	friend COMMON_API void ReleaseCamera(Camera*);

	void BuildFrustumPlanes();

	// no copying
	Camera(const Camera&);
	Camera& operator=(const Camera&);

};

// used to create and release cameras
COMMON_API Camera* CreateCamera();
COMMON_API void ReleaseCamera(Camera*);

#endif // _CAMERA_

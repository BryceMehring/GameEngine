#ifndef _CAMERA_
#define _CAMERA_

#include "CommonExport.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

// Defines a basic camera in screen space
// Note: In order to use these classes, a view matrix must be set by lookAt() along with
// a lens via SetLens()
// update() must be called finally in order to combine the matrices together
class Camera
{
public:

	// Returns the view matrix
	COMMON_API const glm::mat4& View() const; 

	// Returns the projection matrix
	COMMON_API const glm::mat4& Proj() const; 

	// Returns the viewProj matrix (projection * view)
	COMMON_API const glm::mat4& ViewProj() const; 

	// Returns true if the area is visible in the specified area by min and max
	COMMON_API bool IsVisible(const glm::vec3& min, const glm::vec3& max) const;

	// Creates the view matrix
	// pos = position of camera
	// target = where the camera should look
	// up = up direction
	COMMON_API void LookAt(const glm::vec3& pos, const glm::vec3& target = glm::vec3(0.0f), const glm::vec3& up = glm::vec3(0.0f,1.0f,0.0f));

	// Creates an orthogonal matrix
	// fov = not used
	// w = width of the space
	// h = height of the space
	// nearZ = near clip plane
	// farZ = far clip plane
	COMMON_API void SetLens(float fov, float w, float h, float nearZ, float farZ);

	// Updates the Aspect ratio of the view of the camera
	COMMON_API void UpdateAspectRatio(float w, float h);
	
	// Builds ViewProj from the view and projection matrices 
	COMMON_API void Update();

protected:

	// View matrix
	glm::mat4 m_View = glm::mat4(1.0f);

	// Projection matrix
	glm::mat4 m_Proj = glm::mat4(1.0f);

	// Projection * View
	glm::mat4 m_ViewProj = glm::mat4(1.0f);

private:

	glm::vec4 m_planes[6];

	void BuildFrustumPlanes();

};

// Defines a perspective camera in world space
class PerspectiveCamera : public Camera
{
public:

	// Creates a perspective matrix
	// fov = field of view, in radians
	// w = width of the space
	// h = height of the space
	// nearZ = near clip plane
	// farZ = far clip plane
	COMMON_API void SetLens(float fov, float w, float h, float nearZ, float farZ);

	// Updates the Aspect ratio of the view of the camera
	COMMON_API void UpdateAspectRatio(float w, float h);

	// Projects a 2D point into a 3D space
	// pos = the position of the 2D point in screen space with the z value being in the range of [0,1] coming from glReadPixels(...)
	// viewport = {vec2(lower left corner), width, height}
	COMMON_API glm::vec3 UnProjectSS(const glm::vec3& pos, const glm::vec4& viewport) const;

	// Projects a 2D point into a 3D space
	// pos = the position of the 2D point in screen space with the z value being in world space
	// viewport = {vec2(lower left corner), width, height}
	COMMON_API glm::vec3 UnProjectWS(glm::vec3 pos, const glm::vec4& viewport) const;

private:

	float m_fov = 0.0f;

};

#endif // _CAMERA_

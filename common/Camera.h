#ifndef _CAMERA_
#define _CAMERA_

#include "CommonExport.h"
#include <glm/glm.hpp>

// Manages the matrices need to create a camera
// Note: In order to use this class, a view matrix must be set by lookAt() along with
// either a perspective or orthogonal matrix via setLens()
// update() must be called last in order to combine the matrices together
class Camera
{
public:

	COMMON_API Camera();

	// Returns the view matrix
	COMMON_API const glm::mat4& View() const; 

	// Returns the projection matrix
	COMMON_API const glm::mat4& Proj() const; 

	// Returns the viewProj matrix (proj * view)
	COMMON_API const glm::mat4& ViewProj() const; 

	// Returns true if the area is visible in the specified area by min and max
	COMMON_API bool IsVisible(const glm::vec3& min, const glm::vec3& max) const;

	// Creates the view matrix
	// pos = position of camera
	// target = where the camera should look
	// up = which direction is up
	COMMON_API void LookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

	// Creates a perspective or orthogonal matrix
	// fov = field of view
	// w = width of the space
	// h = height of the space
	// nearZ = near clip plane
	// farZ = far clip plane
	COMMON_API void SetLens(float w, float h, float nearZ, float farZ);
	COMMON_API void SetLens(float fov, float w, float h, float nearZ, float farZ);

	// Builds the viewProj from the view matrix and the lens matrix
	COMMON_API void Update();

private:

	glm::mat4 m_View; // View matrix
	glm::mat4 m_Proj; // Projection matrix
	glm::mat4 m_ViewProj; // Projection * View

	glm::vec4 m_planes[6];

	void BuildFrustumPlanes();

};

#endif // _CAMERA_

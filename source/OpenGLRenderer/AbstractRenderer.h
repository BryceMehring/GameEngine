#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "IRenderer.h"
#include "IRenderable.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Mesh.h"
#include <map>
#include <string>
#include <vector>
#include <memory>

#ifdef _MSC_VER
#pragma warning(disable:4503)
#endif

// Manages the rendering of sprites
class AbstractRenderer
{
public:

	AbstractRenderer(ResourceManager* pRm, std::shared_ptr<Mesh> pMesh, Camera* pCam = nullptr);

	void DrawSprite(const std::string& tech,
					const std::string& texture,
					const glm::mat4& transformation,
					const glm::vec4& color,
					const glm::vec2& tiling,
					unsigned int iCellId
				   );

	void DrawString(const char* str,
					const char* font,
					const glm::vec3& pos,
					float scale,
					const glm::vec4& color,
					FontAlignment alignment);

	void DrawLine(const glm::vec3* pArray, // array of 3d vertices to draw
				  unsigned int length, // number of vertices
				  float fWidth, // the width of the line
				  const glm::vec4& color, // color of the line
				  const glm::mat4& t); // transformation to apply to the line

	void SetCamera(Camera* pCam);

	// Renders all of the cached sprites
	void Render();

private:

	ResourceManager* m_pRM;
	std::shared_ptr<Mesh> m_pMesh;

	Camera* m_pCamera;

	// z level -> map of techniques -> map of textures -> vector of sprites
	std::map<int,std::map<std::string,std::map<std::string, std::vector<std::unique_ptr<IRenderable>>>>> m_spriteLayers; // std::vector<std::unique_ptr<IRenderable>>
};

#endif

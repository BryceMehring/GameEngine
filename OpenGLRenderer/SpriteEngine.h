#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "IRenderer.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "VertexBuffer.h"

#include <map>
#include <string>
#include <vector>
#include <memory>

#ifdef _MSC_VER
#pragma warning(disable:4503)
#endif

class IRenderable
{
public:

	virtual ~IRenderable() {}
	virtual void Setup(class ApplyTexturedShader& shader, const IResource &resource) = 0;
	virtual void Render(class ApplyTexturedShader& shader) = 0;
};

// Defines how a sprite should be rendered
class Sprite : public IRenderable
{
public:

	Sprite(const glm::mat4& T, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId);

	void Setup(class ApplyTexturedShader& shader, const IResource &resource) override {}
	void Render(class ApplyTexturedShader& shader) override;

	// Transformation applied to the sprite
	glm::mat4 T;

	// Color blended with the sprite
	glm::vec4 color;

	// The amount of tiling applied to the sprite
	glm::vec2 tiling;

	// Tile of the spite animation
	unsigned int iCellId;
};

// Manages the rendering of sprites
class SpriteEngine
{
public:

	SpriteEngine(ResourceManager* pRm, VertexBuffer* pVertexBuffer, Camera* pCam = nullptr);

	void DrawSprite(const std::string& tech,
					const std::string& texture,
					const glm::mat4& transformation,
					const glm::vec4& color,
					const glm::vec2& tiling,
					unsigned int iCellId
				   );

	void SetCamera(Camera* pCam);

	// Renders all of the cached sprites
	void Render();

private:

	ResourceManager* m_pRM;
	VertexBuffer* m_pVertexBuffer;

	Camera* m_pCamera;

	// z level -> map of techniques -> map of textures -> vector of sprites
	std::map<int,std::map<std::string,std::map<std::string, std::vector<std::unique_ptr<IRenderable>>>>> m_spriteLayers; // std::vector<std::unique_ptr<IRenderable>>
};

#endif

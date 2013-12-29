#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "IRenderer.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "VertexBuffer.h"

#include <map>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable:4503)
#endif

struct Sprite
{
	glm::mat4 T;
	glm::vec4 color;
	glm::vec2 tiling;
	unsigned int iCellId;
};

class SpriteEngine
{
public:

	SpriteEngine(ResourceManager* pRm, VertexBuffer* pVertexStruct, Camera* pCam = nullptr);
	~SpriteEngine();

	void DrawSprite(const std::string& tech,
					const std::string& texture,
					const glm::mat4& transformation,
					const glm::vec4& color,
					const glm::vec2& tiling,
					unsigned int iCellId
				   );

	void Render();

	void SetCamera(Camera* pCam);

private:

	ResourceManager* m_pRM;
	VertexBuffer* m_pVertexBuffer;

	unsigned int m_iCurrentLength;

	Camera* m_pCamera;

	// z level -> map of techniques -> map of textures -> vector of sprites
	std::map<int,std::map<std::string,std::map<std::string,std::vector<Sprite>>>> m_spriteLayers;

	void FillVertexBuffer();
};

#endif

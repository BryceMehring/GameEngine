#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "ResourceManager.h"
#include "Camera.h"

#include <map>
#include <string>
#include <vector>

// todo: move these to another header file
struct SpriteVertex
{
	glm::vec3 pos;
	glm::vec2 tex;
	glm::vec3 color;
	glm::vec2 tiling;
};

struct Sprite
{
	Sprite(const glm::mat4& transform,const glm::vec3& col, const glm::vec2& tile, unsigned int cell)
		: T(transform), color(col), tiling(tile), iCellId(cell)
	{
	}

	glm::mat4 T;
	glm::vec3 color;
	glm::vec2 tiling;
	unsigned int iCellId;
};

struct Layer
{
	typedef std::map<std::string,std::vector<Sprite>> TextureMap;
	std::map<std::string,TextureMap> sprites;
};

class SpriteEngine
{
public:

	SpriteEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~SpriteEngine();

	void DrawSprite(const std::string& tech,
					const std::string& texture,
					const glm::mat4& transformation,
					const glm::vec3& color,
					const glm::vec2& tiling,
					unsigned int iCellId
					);

	void Render();

	void OnReset();

	void SetCamera(Camera* pCam) { m_pCamera = pCam; }

private:


	ResourceManager* m_pRM;

	Camera* m_pCamera;

	unsigned int m_uiVertexBuffer;
	unsigned int m_uiIndexBuffer;

	unsigned int m_iCurrentLength;
	const unsigned int m_iMaxLength;

	std::map<int,Layer> m_spriteLayers;

	void CreateIndexBuffer();
	void CreateVertexBuffer();
	void FillVertexBuffer();

};

#endif

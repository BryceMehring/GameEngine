#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "IRenderer.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "VertexBuffer.h"

#include <map>
#include <string>
#include <vector>

// todo: move these to another header file
struct SpriteVertex
{
	glm::vec3 pos;
	glm::vec2 tex;
	glm::vec3 color;
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

class SpriteEngineBase
{
public:

protected:
};

class SpriteEngine : public SpriteEngineBase
{
public:

	SpriteEngine(ResourceManager* pRm, IndexedVertexBuffer* pVertexStruct, Camera* pCam = nullptr);
	~SpriteEngine();

	void DrawSprite(const std::string& tech,
					const std::string& texture,
					const glm::mat4& transformation,
					const glm::vec3& color,
					const glm::vec2& tiling,
					unsigned int iCellId
				   );

	void Render();

	void SetCamera(Camera* pCam);

private:

	ResourceManager* m_pRM;
	IndexedVertexBuffer* m_pVertexBuffer;

	unsigned int m_iCurrentLength;

	Camera* m_pCamera;
	std::map<int,Layer> m_spriteLayers;

	void FillVertexBuffer();
};

#endif

#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "ResourceManager.h"
#include "Camera.h"

#include <map>
#include <string>
#include <vector>

struct Sprite
{
    Sprite(const glm::mat4& transform, const glm::vec2& tile, unsigned int cell)
        : T(transform), iCellId(cell), tiling(tile)
    {
    }

    glm::mat4 T;
    glm::vec2 tiling;
    unsigned int iCellId;
};

// todo: move these to another header file
struct SpriteVertex
{
    glm::vec3 pos;
    glm::vec2 tex;
    glm::vec2 tiling;
};

class SpriteEngine
{
public:

    SpriteEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
    ~SpriteEngine();

    void DrawSprite(const std::string& tech,
                    const std::string& texture,
                    const glm::mat4& transformation,
                    const glm::vec2& tiling,
                    unsigned int iCellId
                    );

    void Render();

    void OnReset();

private:


    ResourceManager* m_pRM;

    Camera* m_pCamera;

    unsigned int m_uiVertexBuffer;
    unsigned int m_uiIndexBuffer;

    int m_iCurrentLength;
    const unsigned int m_iMaxLength;

    typedef std::map<std::string,std::vector<Sprite>> TextureMap;
    std::map<std::string,TextureMap> m_sprites;
    //std::map<std::string,std::vector<Sprite>> m_sprites;



    void CreateIndexBuffer();
    void CreateVertexBuffer();
    void FillVertexBuffer();

};

#endif

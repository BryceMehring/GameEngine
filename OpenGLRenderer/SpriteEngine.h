#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "ResourceManager.h"
#include "Camera.h"

#include <map>
#include <string>
#include <vector>

struct Sprite
{
    Sprite(const glm::mat4& transform, unsigned int cell, float fDx, float fDy)
    : T(transform), iCellId(cell), dx(fDx), dy(fDy)
    {
    }

    glm::mat4 T;
    unsigned int iCellId;
    float dx;
    float dy;
};

// todo: move these to another header file
struct SpriteVertex
{
    glm::vec3 pos;
    glm::vec2 tex;
};

class SpriteEngine
{
public:

    SpriteEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
    ~SpriteEngine();

    void DrawSprite(const std::string& tech,
                    const glm::mat4& transformation,
                    const std::string& texture,
                    unsigned int iCellId = 0,
                    float dx = 1.0f,
                    float dy = 1.0f);

    void Render();

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

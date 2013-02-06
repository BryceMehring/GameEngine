#ifndef _SPRITEMANAGER_
#define _SPRITEMANAGER_

#include "d3dx9.h"
#include "ResourceManager.h"
#include "Camera.h"

class SpriteManager
{
public:

	SpriteManager(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength);
	~SpriteManager();

	// todo: we need to have technique information about rendering sprites
	void DrawSprite(const std::string& tech, const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId = 0, float dx = 1.0f, float dy = 1.0f);
	void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId = 0, float dx = 1.0f, float dy = 1.0f, DWORD color = 0xffffffff);

	void OnResetDevice();
	void OnLostDevice();

	void Render();

	void SetCamera(Camera* pCam) { m_pCamera = pCam; }

private:

	struct Sprite
	{
		Sprite(const D3DXMATRIX& transform, unsigned int cell, float fDx, float fDy)
		: T(transform), iCellId(cell), dx(fDx), dy(fDy)
		{
		}

		D3DXMATRIX T;
		unsigned int iCellId;
		float dx;
		float dy;
	};

	IDirect3DDevice9* m_pDevice;
	ResourceManager* m_pRM;

	IDirect3DVertexBuffer9* m_pVertexGeoBuffer;
	IDirect3DIndexBuffer9* m_pIndexGeoBuffer;

	Camera* m_pCamera;

	typedef std::map<std::string,std::vector<Sprite>> TextureMap;
	std::map<std::string,TextureMap> m_sprites;
	//std::map<std::string,std::vector<Sprite>> m_sprites;

	int m_iCurrentLength;
	const unsigned int m_iMaxLength;

	void CreateIndexBuffer();
	void FillVertexBuffer();

};

#endif
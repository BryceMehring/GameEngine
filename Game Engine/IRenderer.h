#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include "IResourceManager.h"
#include "VecMath.h"
#include <d3dx9math.h>
#include <string>

class __declspec(novtable) I2DRenderer
{
public:

	virtual ~I2DRenderer() {}

	// Line
	virtual void DrawLine(const D3DXVECTOR3* pArray, unsigned int length) = 0;
	virtual void DrawLine(const D3DXVECTOR3* pArray, unsigned int length, const D3DXVECTOR4& color, const D3DXMATRIX& t) = 0;

	// Fonts
	virtual void GetStringRec(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out) = 0;
	virtual void DrawString(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale = D3DXVECTOR2(4.0f,4.0f), const D3DXVECTOR4& color = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f)) = 0; // world space

	// sprites
	virtual void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId = 0, float dx = 1.0f, float dy = 1.0f, DWORD color = 0xffffffff) = 0;
	//virtual void DrawSpriteAnimation(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iPriority, DWORD color = 0xffffffff);

	
};

class __declspec(novtable) I3DRenderer
{
public:

	virtual ~I3DRenderer() {}

	// todo: need to create an abstract form of vertex declarations

	virtual UINT CreateBillboarderdMesh(unsigned int n, const D3DXVECTOR3* pPosArray, unsigned int uiTech, const std::string& texture) = 0;
	virtual UINT CreateEffectFromFile(const char* file) = 0;
	virtual UINT GetTechnique(UINT uiEffect, const char* Tech) = 0;
	virtual void SetValue(UINT uiEffect, void* pData, UINT bytes) = 0;

	virtual UINT CreateMeshFromFile(const char* file) = 0;
	virtual UINT CreateTriGridMesh() = 0;

	virtual void RenderMesh(UINT) = 0;

};

class __declspec(novtable) IRenderer : public IPlugin
{
public:

	// mesh functions
	/*virtual int LoadMesh(char* pFile) = 0;
	virtual void DrawMesh(int id, int x, int y, int z) = 0;

	//shaders
	virtual int LoadShader(char* pFile) = 0;
	*/
	virtual void ClearScreen() = 0;
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Present() = 0;
	//virtual void DrawSprite();

	virtual void Reset() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual bool IsDeviceLost() = 0;

	// whenever an ISpatialObject is passed to the renderer,
	// the renderer deals with rendering the object so the application does not have to worry about it.
	
	// textures
	virtual IResourceManager& GetResourceManager() = 0;

	virtual I2DRenderer& Get2DRenderer() = 0;
	//virtual I3DRenderer& Get3DRenderer() = 0;

	virtual void SetCamera(class Camera*) = 0;


	// config
	virtual void EnumerateDisplayAdaptors() = 0;
	virtual UINT GetNumDisplayAdaptors() const = 0;
	virtual void SetDisplayMode(UINT i) = 0; // full screen mode
	virtual const std::string& GetDisplayModeStr(UINT i) const = 0;
	virtual void GetWinSize(POINT&) const = 0;
	virtual void ToggleFullscreen() = 0;
	
	///add more functions...
protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

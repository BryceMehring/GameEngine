#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include <d3dx9math.h>
#include <string>

// todo: need to rename IRenderer
class IRenderer : public IPlugin
{
public:

	// mesh functions
	/*virtual int LoadMesh(char* pFile) = 0;
	virtual void DrawMesh(int id, int x, int y, int z) = 0;

	//shaders
	virtual int LoadShader(char* pFile) = 0;
	*/
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Present() = 0;
	//virtual void DrawSprite();

	virtual void Reset() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual bool IsDeviceLost() = 0;

	// Fonts
	virtual void GetStringRec(const char* str, RECT& out) = 0;
	virtual void DrawString(const char* str, POINT P, DWORD color) = 0; // not clipped
	virtual void DrawString(const char* str, RECT& R, DWORD color, bool calcRect = true) = 0; // clipped to rect

	// Line
	virtual void DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color) = 0;
	virtual void DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, D3DXMATRIX* pTransform , D3DCOLOR color) = 0;
	
	
	// todo: need to create an abstract form of vertex declarations

	// Effects
	virtual UINT CreateEffectFromFile(const char* file) = 0;
	virtual UINT GetTechnique(UINT n, const char* name) = 0;
	virtual void SetTechnique(UINT n) = 0;
	virtual void SetValue(void* pData, UINT bytes) = 0;

	// Meshes
	virtual UINT CreateMeshFromFile(const char* file) = 0;
	virtual UINT CreateTriGridMesh() = 0;


	// config
	virtual void EnumerateDisplayAdaptors() = 0;
	virtual void SetDisplayMode(float ratio, unsigned int i) = 0;
	virtual const std::string& GetDisplayModeStr(float ratio, UINT i) const = 0;
	virtual UINT GetModeSize(float ratio) const = 0;
	
	///add more functions...
protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

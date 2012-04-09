#ifndef _IRENDERER_
#define _IRENDERER_

#include "IPlugin.h"
#include <d3dx9math.h>
#include <string>

//FVF quick-system
#define		FVF_XYZ			0x00000001
#define		FVF_NORMAL		0x00000002
#define		FVF_DIFFUSE		0x00000004
#define		FVF_SPECULAR	0x00000008

#define		FVF_TEXTURE0	0x00010000
#define		FVF_TEXTURE1	0x00020000
#define		FVF_TEXTURE2	0x00040000
#define		FVF_TEXTURE3	0x00080000
#define		FVF_TEXTURE(X)	(0x00010000 << (X))

// VertexDecl code from: http://www.gamedev.net/topic/461559-abstracting-directx-vertex-declarations/
//type of element, i.e. what it specifies
enum VertexElementType
{
	VET_XYZ,
	VET_NORMAL,
	VET_DIFFUSE,
	VET_SPECULAR,
	VET_TEXCOORD,
};

//'format' of element, basically the data type
//it's recommended that you avoid the INT64 types, as well as long double
enum VertexElementFormat
{
	VEF_SIGNED_BYTE,
	VEF_UNSIGNED_BYTE,

	VEF_SIGNED_SHORT,
	VEF_UNSIGNED_SHORT,

	VEF_UNSIGNED_INT,
	VEF_SIGNED_INT,

	VEF_UNSIGNED_INT64,
	VEF_SIGNED_INT64,

	VEF_FLOAT,
	VEF_DOUBLE,
	VEF_LONG_DOUBLE,
};

//specifies a complete vertex, basically just a list of elements
struct VertexDeclaration
{
	typedef std::vector<D3DVERTEXELEMENT9> ElementList;
	typedef ElementList::iterator Iterator;

	ElementList Elements;
	unsigned int iClassSize;

	VertexDeclaration()
	{
		Elements.reserve( 4 );
	}
};



// todo: need to rename IRenderer
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

	// Fonts
	virtual void GetStringRec(const char* str, RECT& out) = 0;
	virtual void DrawString(const char* str, POINT P, DWORD color) = 0; // not clipped
	virtual void DrawString(const char* str, RECT& R, DWORD color, bool calcRect = true) = 0; // clipped to rect

	// Line
	virtual void DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color) = 0;
	virtual void DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, D3DXMATRIX* pTransform , D3DCOLOR color) = 0;
	
	
	// todo: need to create an abstract form of vertex declarations

	// Effects, todo: need to abstract this
	virtual UINT CreateEffectFromFile(const char* file) = 0;
	virtual UINT GetTechnique(UINT n, const char* name) = 0;
	virtual void SetTechnique(UINT n) = 0;
	virtual void SetValue(void* pData, UINT bytes) = 0;

	// vertex buffers... todo: need to abstract this
	virtual UINT CreateVertexBuffer(UINT bytes,DWORD flags) = 0;
	virtual void* WriteToVertexBuffer(UINT iBufferIndex) = 0;
	virtual void Unlock(UINT iIndex) = 0;
	virtual void DrawVertexBuffer(UINT iIndex) = 0;

	virtual UINT CreateVertexDecl(const VertexDeclaration& decl) = 0;
	//virtual UINT CreateVertexDecl(

	// Meshes
	virtual UINT CreateMeshFromFile(const char* file) = 0;
	virtual UINT CreateTriGridMesh() = 0;


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

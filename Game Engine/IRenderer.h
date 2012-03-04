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
/*enum VertexElementType
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

//specifies a single part of a vertex, e.g. one position, or one texcoord, etc.
struct VertexElement
{
	unsigned int Stream;
	unsigned int Count;
	std::size_t Offset;		//offset in the structure
	VertexElementFormat Format;
	VertexElementType Type;

	//normal ctor
	VertexElement() : Stream( 0 ), Count( 0 ), Format( VEF_FLOAT ), 
		Type( VET_XYZ ), Offset( 0 )
	{ }

	//inline ctor for laziness
	VertexElement( unsigned int vStream, unsigned int vCount, std::size_t vOffset, 
		VertexElementFormat vFormat, VertexElementType vType )
		: Stream( vStream ), Count( vCount ), Format( vFormat), Type( vType ),
		Offset( vOffset )
	{ }

	static std::size_t FormatSize( VertexElementFormat vef );

	//Compute the size of this element in bytes
	std::size_t SizeBytes() { return FormatSize( Format ) * Count; }
};


//specifies a complete vertex, basically just a list of elements
struct VertexDeclaration
{
	typedef std::vector<VertexElement> ElementList;
	typedef ElementList::iterator Iterator;

	ElementList Elements;

	VertexDeclaration()
	{
		Elements.reserve( 4 );
	}

	static VertexDeclaration CreateFromFVF( unsigned int FVF );
};*/



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

	// vertex buffer
	virtual UINT CreateVertexBuffer(UINT bytes,DWORD flags) = 0;
	virtual void* WriteToVertexBuffer(UINT iBufferIndex) = 0;
	virtual void Unlock(UINT iIndex) = 0;
	virtual void DrawVertexBuffer(UINT iIndex) = 0;
	//virtual UINT CreateVertexDecl(

	// Meshes
	virtual UINT CreateMeshFromFile(const char* file) = 0;
	virtual UINT CreateTriGridMesh() = 0;


	// config
	virtual void EnumerateDisplayAdaptors() = 0;
	virtual UINT GetNumDisplayAdaptors() const = 0;
	virtual void SetDisplayMode(UINT i) = 0;
	virtual const std::string& GetDisplayModeStr(UINT i) const = 0;
	
	///add more functions...
protected:

	virtual ~IRenderer() {}

};


#endif //_IRENDERER_

#include "gassert.h"
#include "DxPolygon.h"
#include "IRenderer.h"
#include "PluginManager.h"
#include "VecMath.h"
#include "FileManager.h"
#include "VecMath.h"

using namespace std;

// ===== DxPolygon =====

DxPolygon::DxPolygon() : m_color(0xffffffff), m_fAngle(0.0f)
{
}
DxPolygon::DxPolygon(const D3DXVECTOR3* pArray, unsigned int size) : m_color(0xffffffff), m_fAngle(0.0f)
{
	ConstructFromArray(pArray,size);
}
DxPolygon::DxPolygon(const std::vector<D3DXVECTOR3>& edges) : m_edges(edges), m_color(0xffffffff), m_fAngle(0.0f)
{
}

const D3DXVECTOR3& DxPolygon::operator[](unsigned int i) const { return m_edges[i]; }
D3DXVECTOR3& DxPolygon::operator[](unsigned int i) { return m_edges[i]; }

// Set
void DxPolygon::SetColor(DWORD color) { m_color = color; }
void DxPolygon::SetAngle(float fAngle) { m_fAngle = fAngle; }

void DxPolygon::ConstructFromArray(const std::vector<D3DXVECTOR3>& edges)
{
	m_edges = edges;
}
void DxPolygon::ConstructFromArray(const D3DXVECTOR3* pArray, unsigned int size)
{
	gassert(pArray != nullptr,"Null pointer");
	//gassert(pArray[0] == pArray[size-1],"DxPolygon is not configured correctly");

	// todo: is this line below needed?
	// If one is constructing a polygon again, most of the time
	// It will be the same shape
	//note: Loop at QuadTree::Render()
	m_edges.clear();
	m_edges.resize(size);

	// copy vectors from array into the dynamic array
	std::copy(pArray,pArray+size,&(m_edges.front()));
}

// Get
DWORD DxPolygon::GetColor() const { return m_color; }
unsigned int DxPolygon::GetSize() const { return m_edges.size(); }
std::vector<D3DXVECTOR3> DxPolygon::GetEdges() const { return m_edges; }

void DxPolygon::Render(IRenderer& renderer)
{
	//renderer.Get2DRenderer().DrawLine(&(m_edges.front()),m_edges.size(),m_fAngle);
}

// algorithm from: http://alienryderflex.com/polygon/
bool DxPolygon::IsPointInPolygon(POINT P) const
{
	return Math::IsPointInPolygon(&(m_edges.front()),m_edges.size(),P);
}

bool DxPolygon::Overlaps(const DxPolygon& other) const
{
	return Math::Sat(m_edges,other.m_edges) && Math::Sat(other.m_edges,m_edges);
}
IRender::IRenderType DxPolygon::GetRenderType() const { return IRender::Polygon; }

// ===== DxSquare =====

DxSquare::DxSquare() {}
DxSquare::DxSquare(const RECT& R)
{
	ConstructFromRect(R);
}
DxSquare::DxSquare(const Math::FRECT& R)
{
	ConstructFromRect(R);
}
DxSquare::DxSquare(const D3DXVECTOR3* pArray, unsigned int size) : DxPolygon(pArray,size)
{
	gassert(size != SIZE,"Incorrect size");
}

void DxSquare::ConstructFromRect(const RECT& R)
{
	D3DXVECTOR3 vec[SIZE];

	vec[0] = D3DXVECTOR3((float)R.right,(float)R.bottom,0.0f);
	vec[1] = D3DXVECTOR3((float)R.right,(float)R.top,0.0f);
	vec[2] = D3DXVECTOR3((float)R.left,(float)R.top,0.0f);
	vec[3] = D3DXVECTOR3((float)R.left,(float)R.bottom,0.0f);
	vec[4] = D3DXVECTOR3((float)R.right,(float)R.bottom,0.0f);

	ConstructFromArray(vec,SIZE);

	m_rect = R;
}

void DxSquare::ConstructFromRect(const Math::FRECT& R)
{
	D3DXVECTOR3 vec[SIZE];

	vec[0] = D3DXVECTOR3(R.bottomRight.x,R.bottomRight.y,0.0f);
	vec[1] = D3DXVECTOR3(R.bottomRight.x,R.topLeft.y,0.0f);
	vec[2] = D3DXVECTOR3(R.topLeft.x,R.topLeft.y,0.0f);
	vec[3] = D3DXVECTOR3(R.topLeft.x,R.bottomRight.y,0.0f);
	vec[4] =D3DXVECTOR3(R.bottomRight.x,R.bottomRight.y,0.0f);

	ConstructFromArray(vec,SIZE);
}

const RECT& DxSquare::GetRect() const
{
	return m_rect;
}

/*void DxSquare::SendMsg(int msg)
{
	switch(msg)
	{
	case 0:

		RECT R;
		GetWindowRect(GetActiveWindow(),&R);

		RECT minus;
		SubtractRect(&minus,&R,&m_rect);

		m_rect.left += minus.left;
		m_rect.top += minus.top;
		m_rect.bottom += minus.bottom;
		m_rect.right += minus.right;

		ConstructFromRect(m_rect);

		break;
	}
}*/

bool DxSquare::IsPointInPolygon(POINT P) const
{
	return PtInRect(&m_rect,P) > 0;
}

// ==== DxTriangle ====

DxTriangle::DxTriangle() {}
DxTriangle::DxTriangle(const D3DXVECTOR3* pArray, unsigned int size) : DxPolygon(pArray,size) 
{
}


// algorithm from: http://www.blackpawn.com/texts/pointinpoly/default.html
bool DxTriangle::IsPointInPolygon(POINT P) const
{
	D3DXVECTOR3 point((float)P.x,(float)P.y,0.0f);

	// Compute vectors        
	D3DXVECTOR3 v0 = m_edges[2] - m_edges[0];
	D3DXVECTOR3 v1 = m_edges[1] - m_edges[0];
	D3DXVECTOR3 v2 = point - m_edges[0];

	// Compute dot products
	float dot00 = D3DXVec3Dot(&v0, &v0);
	float dot01 = D3DXVec3Dot(&v0, &v1);
	float dot02 = D3DXVec3Dot(&v0, &v2);
	float dot11 = D3DXVec3Dot(&v1, &v1);
	float dot12 = D3DXVec3Dot(&v1, &v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}


#include <assert.h>
#include "DxPolygon.h"
#include "IRenderer.h"
#include "PluginManager.h"

using namespace std;

// ===== DxPolygon =====

DxPolygon::DxPolygon()
{
}
DxPolygon::DxPolygon(const D3DXVECTOR2* pArray, unsigned int size)
{
	ConstructFromArray(pArray,size);
}

const D3DXVECTOR2& DxPolygon::operator[](unsigned int i) const { return m_edges[i]; }
D3DXVECTOR2& DxPolygon::operator[](unsigned int i) { return m_edges[i]; }

// Set
void DxPolygon::SetColor(DWORD color) { m_color = color; }

void DxPolygon::ConstructFromArray(const D3DXVECTOR2* pArray, unsigned int size)
{
	assert(pArray != nullptr);
	assert(pArray[0] == pArray[size-1]);

	m_edges.clear();
	m_edges.resize(size);

	for(unsigned int i = 0; i < size; ++i)
	{
		m_edges[i] = pArray[i];
	}
}

// Get
DWORD DxPolygon::GetColor() const { return m_color; }
unsigned int DxPolygon::GetSize() const { return m_edges.size(); }

void DxPolygon::Render(IRenderer* pRenderer)
{
	pRenderer->DrawLine(&(m_edges.front()),m_edges.size(),m_color);
}

// algorithm from: http://alienryderflex.com/polygon/
bool DxPolygon::IsPointInPolygon(POINT P) const
{
	int j= m_edges.size()-1;
	bool oddNodes = false;

	for(unsigned int i=0; i < m_edges.size(); i++)
	{
		if ((m_edges[i].y< P.y && m_edges[j].y>= P.y ||   m_edges[j].y< P.y && m_edges[i].y>=P.y) &&  (m_edges[i].x<=P.x || m_edges[j].x<=P.x))
		{
			oddNodes^=(m_edges[i].x+(P.y-m_edges[i].y)/(m_edges[j].y-m_edges[i].y)*(m_edges[j].x-m_edges[i].x)<P.x);
		}
		j=i;
	}

	return oddNodes; 
}
IRender::IRenderType DxPolygon::GetRenderType() const { return IRender::Polygon; }

// ===== DxSquare =====

DxSquare::DxSquare() {}
DxSquare::DxSquare(const RECT& R)
{
	ConstructFromRect(R);
}
DxSquare::DxSquare(const D3DXVECTOR2* pArray, unsigned int size) : DxPolygon(pArray,size) 
{
	assert(size != 5);
}

void DxSquare::ConstructFromRect(const RECT& R)
{
	D3DXVECTOR2 vec[5];

	vec[0] = D3DXVECTOR2((float)R.right,(float)R.bottom);
	vec[1] = D3DXVECTOR2((float)R.right,(float)R.top);
	vec[2] = D3DXVECTOR2((float)R.left,(float)R.top);
	vec[3] = D3DXVECTOR2((float)R.left,(float)R.bottom);
	vec[4] = D3DXVECTOR2((float)R.right,(float)R.bottom);

	ConstructFromArray(vec,5);
}

bool DxSquare::IsPointInPolygon(POINT P) const
{
	if((P.x <= m_edges[0].x) && (P.x >= m_edges[2].x))
	{
		if((P.y <= m_edges[0].y) && (P.y >= m_edges[2].y))
		{
			return true;
		}
	}

	return false;
}

// ==== DxTriangle ====

DxTriangle::DxTriangle() {}
DxTriangle::DxTriangle(const D3DXVECTOR2* pArray, unsigned int size) : DxPolygon(pArray,size) 
{
	assert(size != 4);
}

// algorithm from: http://www.blackpawn.com/texts/pointinpoly/default.html
bool DxTriangle::IsPointInPolygon(POINT P) const
{
	D3DXVECTOR2 point((float)P.x,(float)P.y);

	// Compute vectors        
	D3DXVECTOR2 v0 = m_edges[2] - m_edges[0];
	D3DXVECTOR2 v1 = m_edges[1] - m_edges[0];
	D3DXVECTOR2 v2 = point - m_edges[0];

	// Compute dot products
	float dot00 = D3DXVec2Dot(&v0, &v0);
	float dot01 = D3DXVec2Dot(&v0, &v1);
	float dot02 = D3DXVec2Dot(&v0, &v2);
	float dot11 = D3DXVec2Dot(&v1, &v1);
	float dot12 = D3DXVec2Dot(&v1, &v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}


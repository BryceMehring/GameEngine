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

void DxPolygon::Render()
{
	PluginManager& instance = PluginManager::Instance();
	IRenderer* pRenderer = static_cast<IRenderer*>(instance.GetPlugin(Rendering));
	pRenderer->DrawLine(&(m_edges.front()),m_edges.size(),m_color);
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

bool DxSquare::IsPointInPolygon(POINT P)
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
#ifndef _DXPOLYGON_
#define _DXPOLYGON_

#include "Interfaces.h"

class DxPolygon : public IRender
{
public:

	DxPolygon();
	DxPolygon(const D3DXVECTOR2* pArray, unsigned int size);
	virtual ~DxPolygon() {}

	// array access
	const D3DXVECTOR2& operator[](unsigned int i) const;
	D3DXVECTOR2& operator[](unsigned int i);

	// Set
	void SetColor(DWORD color);

	// resize
	void ConstructFromArray(const D3DXVECTOR2* pArray, unsigned int size);

	// Get
	DWORD GetColor() const;
	unsigned int GetSize() const;
	virtual IRenderType GetRenderType();

	// Operations
	virtual void Render(IRenderingPlugin& renderer);	

protected:
	std::vector<D3DXVECTOR2> m_edges;
	DWORD m_color;
};

class DxSquare : public DxPolygon
{
public:

	DxSquare();
	DxSquare(const RECT& R);
	DxSquare(const D3DXVECTOR2* pArray, unsigned int size);

	void ConstructFromRect(const RECT& R);

};

#endif
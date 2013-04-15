#ifndef _DXPOLYGON_
#define _DXPOLYGON_

#include "IRender.h"
#include <d3dx9math.h>
#include <vector>

class DxPolygon : public IRender
{
public:

	DxPolygon();
	DxPolygon(const D3DXVECTOR3* pArray, unsigned int size);
	explicit DxPolygon(const std::vector<D3DXVECTOR3>& edges);
	virtual ~DxPolygon() {}

	// array access
	const D3DXVECTOR3& operator[](unsigned int i) const;
	D3DXVECTOR3& operator[](unsigned int i);

	// Set
	void SetColor(DWORD color);
	void SetAngle(float angle);

	// resize
	void ConstructFromArray(const std::vector<D3DXVECTOR3>& edges);
	void ConstructFromArray(const D3DXVECTOR3* pArray, unsigned int size);

	// Get
	DWORD GetColor() const;
	std::vector<D3DXVECTOR3> GetEdges() const;
	unsigned int GetSize() const;
	virtual IRenderType GetRenderType() const;

	// Operations
	virtual void Render(class IRenderer&);

	// Ray casting algorithm
	virtual bool IsPointInPolygon(POINT P) const;

	bool Overlaps(const DxPolygon& other) const;

protected:
	std::vector<D3DXVECTOR3> m_edges;
	float m_fAngle;
	DWORD m_color;
};


// Forward Decl
namespace Math
{
	
struct FRECT;

}

// todo: create diff kinds of squares
class DxSquare : public DxPolygon
{
public:

	DxSquare();
	DxSquare(const Math::FRECT& R);
	DxSquare(const RECT& R);
	DxSquare(const D3DXVECTOR3* pArray, unsigned int size);

	void ConstructFromRect(const RECT& R);
	void ConstructFromRect(const Math::FRECT& R);
	const RECT& GetRect() const;

	virtual bool IsPointInPolygon(POINT P) const; 

private:

	RECT m_rect;
	static const int SIZE = 5;
};

class DxTriangle : public DxPolygon
{
public:

	DxTriangle();
	DxTriangle(const D3DXVECTOR3* pArray, unsigned int size);

	virtual bool IsPointInPolygon(POINT P) const;

private:

	static const int SIZE = 4;

};


#endif
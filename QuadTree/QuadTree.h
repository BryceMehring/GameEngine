// Programmed by Bryce Mehring
// todo: need to clean up the interfaces.

#ifndef _QUADTREE_
#define _QUADTREE_

#include <set>
#include <vector>
#include <list>
#include <assert.h>
#include "VecMath.h"
#include "IRender.h"
#include "ISpatialObject.h"

// The Quadtree manages objects in 2D space

class QuadTree : public IRender
{
public:

	QuadTree(const Math::FRECT& R, unsigned int nodeCapacity);

	// Inserts obj into the quadtree
	// returns true if obj was inserted into the quadtree, else false
	bool Insert(ISpatialObject& obj);

	// Removes obj from the quadtree
	void Erase(ISpatialObject& obj);

	// Returns all objects in the quadtree that collide with pObj or poly via the out parameter
	void QueryNearObjects(const ISpatialObject* pObj, std::vector<ISpatialObject*>& out);
	void QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out);

	// Returns true if obj lies within the quadtree
	bool IsWithin(ISpatialObject& obj) const;

	// Returns the quadtree rectangle
	const Math::FRECT& GetRect() const;

	// Draws the quadtree rectangles as lines
	void Render(IRenderer& renderer) override;

private:

	QuadTree(const Math::FRECT& rect, unsigned int nodeCapacity, unsigned int height, QuadTree* pPrevious);

	// Returns true if this node is divided
	bool IsDivided() const;

	// Returns true if this node has objects inserted into it
	bool HasObjects() const;

	// Returns true if this part of the quadtree has reached its capacity
	bool IsFull() const;

	// Subdivides the current node into 4 sub nodes
	void SubDivide();

	void QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out, const ISpatialObject* pObj);

private:

	typedef std::vector<ISpatialObject*> LIST_DTYPE;

	friend class NodeIterator;

	Math::CRectangle m_Rect;
	LIST_DTYPE m_Objects;

	std::vector<QuadTree> m_Nodes;
	QuadTree* const m_Previous;

	unsigned int m_iCapacity;
	const unsigned int m_iHeight;
};

#endif // _QUADTREE_

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

class ISpatialObject : public IRender
{
public:

	virtual ~ISpatialObject() {}

	virtual const glm::vec3& GetPos() const = 0;
	virtual glm::vec2 GetDir() const = 0;
	virtual float GetSpeed() const = 0;

	virtual void* QueryInterface(unsigned int) const = 0;

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const = 0;
};

class QuadTree : public IRender
{
public:

	QuadTree(const Math::FRECT& R, unsigned int nodeCapacity);

	// removes obj from the quadtree
	void Erase(ISpatialObject& obj);

	// returns true if obj lies within the quadtree
	bool IsWithin(ISpatialObject& obj) const;

	// operations
	bool Insert(ISpatialObject& obj);

	// returns all objects in the quadtree that collide with pObj or poly via the out parameter
	void QueryNearObjects(const ISpatialObject* pObj, std::vector<ISpatialObject*>& out);
	void QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out);

	// Draws the quadtree rectangles
	void Render(IRenderer& renderer) override;

	// Returns the quadtree rectangle
	const Math::FRECT& GetRect() const { return m_Rect.GetRect(); }

private:

	QuadTree(const Math::FRECT& rect, unsigned int nodeCapacity, unsigned int height, QuadTree* pPrevious);

	void FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<QuadTree*>& out);

	// recursive insertion algorithm
	void RInsert(ISpatialObject& obj);

	// Returns true if this node is divided
	bool IsDivided() const;

	// Returns true if this node has objects inserted into it
	bool HasObjects() const;
	bool IsFull() const;

	// Subdivides the current node/R into 4 sub nodes
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

class NodeIterator
{
public:

	NodeIterator(QuadTree* pNode = nullptr);

	NodeIterator& operator=(QuadTree* pNode);
	NodeIterator& operator++();
	NodeIterator& operator++(int unused);

	bool operator ==(QuadTree* pNode);
	bool operator !=(QuadTree* pNode);

	bool operator ==(const NodeIterator&);
	bool operator !=(const NodeIterator&);

	QuadTree* operator*();
	QuadTree* operator->();

protected:

	QuadTree* m_pNode;

	unsigned int GetIndex() const;
	void Increment();
	void LoopDown(unsigned int index);
	void LoopUp();
};

#endif // _QUADTREE_

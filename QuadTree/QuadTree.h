// Programmed by Bryce Mehring
// 1/2/2012

// todo: need to clean up the interfaces.
// todo: need to update the quadtree so that it grows upwards when no objects are in the node.

#ifndef _QUADTREE_
#define _QUADTREE_

#include <set>
#include <vector>
#include <list>
#include <assert.h>
#include "IRender.h"
#include "VecMath.h"

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

class QuadTree
{
public:

	typedef std::list<ISpatialObject*> LIST_DTYPE;

	friend class NodeIterator;

	// constructors
	QuadTree();
	QuadTree(const Math::FRECT& R);
	~QuadTree();

	void Erase(ISpatialObject& obj);

	// returns true if P lies within the rectangle
	bool IsWithin(ISpatialObject& obj) const;

	// operations
	bool Insert(ISpatialObject& obj);

	void QueryNearObjects(const ISpatialObject* pObj, std::vector<ISpatialObject*>& out);
	void QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out);

	void Render(IRenderer& renderer);

	const Math::FRECT& GetRect() const { return R.GetRect(); }

private:

	// The rectangle of the current node
	Math::CRectangle R;
	LIST_DTYPE m_Objects;

	std::vector<QuadTree> m_Nodes;
	QuadTree* m_Previous;

	int m_iHeight;

	// --- helper functions ---

	void FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<QuadTree*>& out);

	// recursive insertion algorithm
	void RInsert(ISpatialObject& obj);

	// Returns true if this node is divided.
	bool IsDivided() const;

	bool HasPoint() const;
	bool IsFull() const;

	// Subdivides the current node/R into 4 sub nodes
	void SubDivide();

	void QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out, const ISpatialObject* pObj);
};

// todo: need to create a better public/protected/private interface
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
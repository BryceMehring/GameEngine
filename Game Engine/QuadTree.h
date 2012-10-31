// Programmed by Bryce Mehring
// 1/2/2012

// todo: need to clean up the interfaces.
// todo: need to update the quadtree so that it grows upwards when no objects are in the node.

#ifndef _QUADTREE_
#define _QUADTREE_
#pragma once

#include <Windows.h>
#include <set>
#include <vector>
#include <assert.h>
#include "IRender.h"
#include "RTTI.h"
#include "VecMath.h"

const unsigned int MAX_NODES = 4;
const unsigned int MAX_OBJ_PERNODE = 10;

class NodeIterator;
class ISpatialObject;

class Node
{
public:

	typedef std::set<ISpatialObject*> LIST_DTYPE;

	friend class NodeIterator;

	// constructors
	Node();
	Node(const Math::FRECT& R);
	~Node();

	void SetRect(const Math::CRectangle& R); 

	void Erase(ISpatialObject& obj);
	void EraseFromPreviousPos(ISpatialObject& obj);

	// todo: return reference
	LIST_DTYPE* GetNearObjects() { return &m_Objects; }

	// returns true if P lies within the rectangle
	bool IsWithin(ISpatialObject& obj) const;

	// operations
	bool Insert(ISpatialObject& obj);

	// recursive algorithm
	void FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<Node*>& out);

	void Render(IRenderer& renderer);

	const Math::FRECT& GetRect() const { return R.GetRect(); }
	 
private:

	// The rectangle of the current node
	Math::CRectangle R;
	//std::list<obj*>* m_pObjects;
	// obj
	LIST_DTYPE m_Objects;
	//KEY* m_pKey;

	// the sub nodes of the current node
	Node* m_Nodes[MAX_NODES];
	Node* m_Previous;

	// --- helper functions ---

	// recursive insertion algorithm
	void RInsert(ISpatialObject& obj);

	// Returns true if this node is divided. 
	bool IsDivided() const;

	bool HasPoint() const;
	bool RHasPoint() const;
	bool IsFull() const;

	// Subdivides the current node/R into 4 sub nodes
	void SubDivide();
	void SubDivideObjects(Node& subNode);
	void ExpandLeft();
	void ExpandRight();
};

class ISpatialObject : public IRender
{
public:

	friend class Node;

	virtual ~ISpatialObject() {}

	virtual D3DXVECTOR2 GetPos() const = 0;
	virtual D3DXVECTOR2 GetDir() const = 0;
	virtual float GetSpeed() const = 0;

	virtual void* QueryInterface(unsigned int) const = 0; 

	virtual const Math::ICollisionPolygon& GetCollisionPolygon() const = 0;

protected:

	std::vector<Node*> m_nodes;
};

// todo: need to create a better public/protected/private interface
class NodeIterator
{
public:

	NodeIterator(Node* pNode = nullptr);
		
	NodeIterator& operator=(Node* pNode);
	NodeIterator& operator++();
	NodeIterator& operator++(int unused);

	bool operator ==(Node* pNode);
	bool operator !=(Node* pNode);

	bool operator ==(const NodeIterator&);
	bool operator !=(const NodeIterator&);

	Node* operator*();
	Node* operator->();

protected:

	Node* m_pNode;

	unsigned int GetIndex() const;
	void Increment();
	void LoopDown(unsigned int index);
	void LoopUp();
};

class QuadTree : public IRender
{
public:

	// constructor/destructor
	QuadTree(const Math::FRECT& R);
	virtual ~QuadTree();

	// adds a point to the quadtree
	// recursive
	bool Insert(ISpatialObject&);
	void Erase(ISpatialObject&);
	void EraseFromPrev(ISpatialObject&);

	bool IsWithin(ISpatialObject&) const;

	void FindNearObjects(Math::ICollisionPolygon* pPoly,std::vector<ISpatialObject*>& out);
	void FindNearNodes(Math::ICollisionPolygon* pPoly, std::vector<Node*>& out);
	void FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out);
	//void FindNearNodes(const ISpatialObject* pObj, std::vector<Node*>& out);

	template< class T >
	void Update(ISpatialObject& obj, const T& funct)
	{
		Erase(obj);
		funct();
		Insert(obj);
	}

	void Update(ISpatialObject& obj);

	void SaveToFile(std::string& file);
	void LoadFile(const std::string& file);

	virtual void Render(class IRenderer&);

	const Math::FRECT& GetRect() const { return m_pRoot->GetRect(); }

private:
	Node* m_pRoot;
};

// helper functions
template< class T >
void ProccessNearNodes(const std::vector<Node*>& nodes, const T& functor)
{
	// this set is used to only process each object once
	std::set<ISpatialObject*> outSet;

	// loop over all of the nodes the object collides with 
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		// Find the near objects to the node
		auto theList = nodes[i]->GetNearObjects();

		// Loop over all the objects in the node
		for(auto iter = theList->begin(); iter != theList->end(); ++iter)
		{
			ISpatialObject* pObj = *iter;

			// If this is the first pObj being processed 
			if(outSet.insert(pObj).second == true)
			{
				// process this object
				if(functor(pObj)) { return; }
			}
		}
	}
}


#endif // _QUADTREE_
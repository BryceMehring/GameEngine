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
	Node(const FRECT& R);
	~Node();

	void SetRect(const CRectangle& R); 

	void Erase(ISpatialObject* pObj);
	void EraseFromPreviousPos(ISpatialObject* pObj);

	const LIST_DTYPE* GetNearObjects() const { return m_pObjects; }

	// returns true if P lies within the rectangle
	bool IsWithin(const ISpatialObject* pObj) const;

	// operations
	bool Insert(ISpatialObject* pObj);

	// recursive algorithm
	void FindNearNodes(const ICollisionPolygon* pObj, std::vector<Node*>& out);

	void Render(IRenderer* pRenderer);

	const FRECT& GetRect() const { return R.GetRect(); }
	 
private:

	// The rectangle of the current node
	CRectangle R;
	//std::list<obj*>* m_pObjects;
	// obj
	LIST_DTYPE* m_pObjects;
	//KEY* m_pKey;

	// the sub nodes of the current node
	Node* m_Nodes[MAX_NODES];
	Node* m_Previous;

	bool m_bUseable;

	// --- helper functions ---

	// recursive insertion algorithm
	void RInsert(ISpatialObject* pObj);

	// Returns true if this node is divided. 
	bool IsDivided() const;

	bool HasPoint() const;
	bool RHasPoint() const;
	bool IsFull() const;

	// Subdivides the current node/R into 4 sub nodes
	void SubDivide(bool bAlloc);
	void SubDivideObjects(Node* pSubNode);
	void ExpandLeft();
	void ExpandRight();
};

class ISpatialObject : public IRender
{
public:

	friend class Node;

	enum Type
	{
		Unit,
		AnotherUnit,
	};

	ISpatialObject() : m_pCollisionPolygon(nullptr) {}

	// todo: need some rtti info here for casting
	virtual ~ISpatialObject()
	{
		delete m_pCollisionPolygon;
	}

	//-todo: need to change the KEY structure to D3dxvector?
	virtual D3DXVECTOR2 GetPos() const = 0;
	virtual Type GetType() const = 0;

	// todo: this method is not needed
	void EraseFromQuadtree(class QuadTree* pTree);

	// todo: create a better interface
	void SetCollisionPolygon(ICollisionPolygon* pPolygon) { m_pCollisionPolygon = pPolygon; }
	const ICollisionPolygon* GetCollisionPolygon() const { return m_pCollisionPolygon; }



protected:

	ICollisionPolygon* m_pCollisionPolygon;

	std::vector<Node*> m_nodes;
};

/*class CollidableObject : public ISpatialObject
{
public:

	void foo()
	{
		
	}

	void SetNode(Node* pNode)
	{
		m_pNode = pNode;
	}

protected:
	
	Node* m_pNode; // 
	KEY m_pos; // current position of the object
};*/


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
	QuadTree(const FRECT& R);
	virtual ~QuadTree();

	// adds a point to the quadtree
	// recursive
	bool Insert(ISpatialObject* pObj);
	void Erase(ISpatialObject* pObj);
	void EraseFromPrev(ISpatialObject*);

	bool IsWithin(ISpatialObject* pObj) const;

	// todo: are these methods needed? I guess so, if you want to check for near nodes
	// in different areas
	void FindNearNodes(ICollisionPolygon* pPoly, std::vector<Node*>& out);
	void FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out);
	//void FindNearNodes(const ISpatialObject* pObj, std::vector<Node*>& out);

	void Update(ISpatialObject* pObj);

	void SaveToFile(std::string& file);
	void LoadFile(const std::string& file);

	virtual void Render(class IRenderer* pRenderer);

	const FRECT& GetRect() const { return m_pRoot->GetRect(); }

private:
	Node* m_pRoot;
};

// helper functions
template< class T >
void ProccessNearNodes(const std::vector<Node*>& nodes, const T& functor)
{
	// this set is used to only process each object once
	std::set<const ISpatialObject*> outSet;

	// loop over all of the nodes the object collides with 
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		// Find the near objects to the node
		auto theList = nodes[i]->GetNearObjects();

		//if(theList->size() > 1)
		{
			// Loop over all the objects in the node
			for(auto iter = theList->begin(); iter != theList->end(); ++iter)
			{
				const ISpatialObject* pObj = *iter;

				// If this is the first pObj being processed 
				if(outSet.insert(pObj).second == true)
				{
					// process this object
					if(functor(pObj)) { return; }
				}
			}
		}
	}
}

/*template< class T >
void ProccessNearNodes(Node* pNode, const T& functor)
{
	// this set is used to only process each object once
	std::set<const ISpatialObject*> outSet;

	// loop over all of the nodes the object collides with 
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		Node* pNode = nodes[i];

		// Find the near objects to the node
		auto theList = pNode->GetNearObjects();

		// Loop over all the objects in the node
		for(auto iter = theList->begin(); iter != theList->end(); ++iter)
		{
			const ISpatialObject* pObj = *iter;
			// If this is the first pObj being processed 
			auto pair = outSet.insert(pObj);
			if(pair.second == true)
			{
				// process this object
				if(functor(pObj)) { return; }
			}
		}
	}
}*/




/*class Unit : public CollidableObject
{
public:

	virtual void GetPos(KEY& out)
	{
		out = key;
	}

private:

};

class ISpatialDatastruct
{
public:

	virtual ~ISpatialDatastruct() {}
	virtual void AddObj(ISpatialObject*) = 0;
	virtual void TestColision(ISpatialObject*) = 0;
};

class QuadTreeCollisionDection : public ISpatialDatastruct
{
public:

	QuadTreeCollisionDection(QuadTree* pTree) : m_pTree(pTree)
	{
	}

	virtual ~QuadTreeCollisionDection()
	{
		delete m_pTree;
	}

	virtual void AddObj(ISpatialObject* pObj)
	{
		m_pTree->AddPoint();
	}
	virtual void TestColision(ISpatialObject* pObj)
	{
		KEY pos;
		pObj->GetPos(pos);

		std::list<KEY> points;
		bool success = m_pTree->FindNearPoint(pos,points);

		if(success)
		{
			
		}
	}

private:

	QuadTree* m_pTree;

};*/

#endif // _QUADTREE_
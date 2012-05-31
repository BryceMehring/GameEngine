// Programmed by Bryce Mehring
// 1/2/2012

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

	void Erase(ISpatialObject* pObj)
	{ 
		m_pObjects->erase(pObj);
	}
	const LIST_DTYPE* GetNearObjects() const { return m_pObjects; }

	// operations
	bool Insert(ISpatialObject* pObj);

	// recursive algorithm
	void FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out);

	void Render(IRenderer* pRenderer);

	void Update();

	// todo: need to implement
	//void AddObject(Object* pObject);
	 
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

	// --- helper functions ---

	// recursive insertion algorithm
	void RInsert(ISpatialObject* pObj);

	void Update(Node* pNode);

		// returns true if P lies within the rectangle
	bool IsWithin(const ISpatialObject* pObj) const;

	// Returns true if this node is divided. 
	bool IsDivided() const;

	bool HasPoint() const;
	bool IsFull() const;

	// Subdivides the current node/R into 4 sub nodes
	void SubDivide();
	void ExpandLeft();
	void ExpandRight();
};

class ISpatialObject : public IRender
{
public:

	enum Type
	{
		Unit,
		AnotherUnit,
	};

	ISpatialObject() : m_pCollisionPolygon(nullptr) {}

	// todo: need some rtti info here for casting
	virtual ~ISpatialObject()
	{
		for(unsigned int i = 0; i < m_nodes.size(); ++i)
		{
			m_nodes[i]->Erase(this);
			//m_pNode->Erase(this);
		}
		delete m_pCollisionPolygon;
	}

	// todo: need to change the KEY structure to D3dxvector?
	virtual D3DXVECTOR2 GetPos() const = 0;
	virtual Type GetType() const = 0;

	// todo: create a better interface

	bool HasNode() const
	{ 
		return (!m_nodes.empty());
	}
	void SetNodes(const std::vector<Node*>& nodes)
	{
		m_nodes = nodes;
	}
	void AddNode(Node* pNode) { m_nodes.push_back(pNode); }
	void ClearNodes()
	{
		for(unsigned int i = 0; i < m_nodes.size(); ++i)
		{
			m_nodes[i]->Erase(this);
			//m_pNode->Erase(this);
		}
		m_nodes.clear();
	}
	void SetCollisionPolygon(ICollisionPolygon* pPolygon) { m_pCollisionPolygon = pPolygon; }
	const ICollisionPolygon* GetCollisionPolygon() const { return m_pCollisionPolygon; }

	const std::vector<Node*>& GetNode() const { return m_nodes; }

protected:

	std::vector<Node*> m_nodes;
	ICollisionPolygon* m_pCollisionPolygon;
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

	//typedef typename N::OBJ_TYPE OBJ_TYPE;

	// constructor/destructor
	QuadTree(const FRECT& R);
	virtual ~QuadTree();

	// adds a point to the quadtree
	// non-recursive
	bool Insert(ISpatialObject* pObj);
	void Erase(ISpatialObject* pObj);

	// returns a nearest point in the tree to P
	// non-recursive
	void FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out);
	void FindNearNode(const D3DXVECTOR2& topLeft, const D3DXVECTOR2& bottomRight, std::vector<Node*>& out);

	Node* FindNearNode(ISpatialObject* pObj) const;

	// This function will update the nodes in the quadtree if any of the
	// nodes have left their rectangles.
	// non-recursive
	void Update();
	void Update(Node* pNode);
	//Node* Update(Node* pNode);

	void SaveToFile(std::string& file);
	void LoadFile(const std::string& file);

	virtual void Render(class IRenderer* pRenderer);

private:

	Node* m_pRoot;
	unsigned int m_iMaxSubDivisions;
	mutable unsigned int m_iDepth;

	// todo: need to implement 
	void CalculateMaxSubDivisions();
};

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
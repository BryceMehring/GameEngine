// Programmed by Bryce Mehring
// 1/2/2012

#ifndef _QUADTREE_
#define _QUADTREE_
#pragma once

#include <Windows.h>
#include <set>
#include <hash_map>
#include "IRender.h"

const int MAX_NODES = 4;
const int MAX_OBJ_PERNODE = 10;

class NodeIterator;
class ISpatialObject;
typedef POINT KEY;



class Node
{
public:

	typedef std::set<ISpatialObject*> LIST_DTYPE;

	// friends
	//template< class N>
	//friend class QuadTree<N>;
	friend class QuadTree;
	friend class NodeIterator;

	// constructors
	Node();
	Node(const RECT& R);
	~Node();

	void SetRect(const RECT& R);

	void Erase(ISpatialObject* pObj) { m_pKeys->erase(pObj); }
	const LIST_DTYPE* GetNearObjects() const { return m_pKeys; }

	// returns true if P lies within the rectangle
	bool IsPointWithin(KEY P) const;
	bool IsPointWithin(const ISpatialObject* pObj) const;

	// Returns true if this node is divided. 
	bool IsDivided() const;

	bool HasPoint() const;
	bool IsFull() const;

	// todo: need to implement
	//void AddObject(Object* pObject);
	 
private:

	// The rectangle of the current node
	RECT R;
	//std::list<obj*>* m_pKeys;
	// obj
	
	LIST_DTYPE* m_pKeys;
	//KEY* m_pKey;

	// the sub nodes of the current node
	Node* m_Nodes[MAX_NODES];
	Node* m_Previous;

	// Helper functions
	void AssignRect(const RECT& R);

	// Subdivides the current node/R into 4 sub nodes
	void SubDivide();
	void ExpandLeft();
	void ExpandRight();
};

class ISpatialObject
{
public:

	enum Type
	{
		Unit,
		AnotherUnit,
	};

	ISpatialObject() : m_pNode(nullptr) {}

	// todo: need some rtti info here for casting
	virtual ~ISpatialObject()
	{
		if(m_pNode)
		{
			m_pNode->Erase(this);
		}
	}
	virtual const KEY& GetPos() const = 0;
	virtual Type GetType() const = 0;

	void SetNode(Node* pNode) { m_pNode = pNode; }

protected:

	Node* m_pNode; 
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

class BaseIterator
{
public:

	BaseIterator& operator=(const Node* pNode);
	BaseIterator& operator++();
	BaseIterator& operator++(int unused);

	Node* operator*();
	Node* operator->();

protected:

	Node* m_pNode;

	void LoopDown(unsigned int index);

};

// todo: need to create a better public/protected/private interface
class NodeIterator
{
public:

	NodeIterator(Node* pNode = nullptr);
		
	NodeIterator& operator=(const Node* pNode);
	NodeIterator& operator++();
	NodeIterator& operator++(int unused);

	Node* operator*();
	Node* operator->();

protected:

	Node* m_pNode;

	unsigned int GetIndex() const;
	void Increment();
	void LoopDown(unsigned int index);
	void LoopUp();
};

class PointIterator : public NodeIterator
{
public:

	PointIterator(Node* pNode = nullptr);

protected:

	void Increment();

};

class BaseQuadTree
{
public:

protected:
};

class PointQuadTree
{
public:



};

class QuadTree : public IRender
{
public:

	//typedef typename N::OBJ_TYPE OBJ_TYPE;

	// constructor/destructor
	QuadTree(const RECT& R);
	virtual ~QuadTree();

	// adds a point to the quadtree
	// non-recursive
	bool Insert(ISpatialObject* pObj);
	void Erase(ISpatialObject* pObj);

	// returns a nearest point in the tree to P
	// non-recursive
	Node* FindNearNode(const KEY& P) const;
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

	Node* FindNearNode(const KEY& P, Node* pNode) const;

	bool Insert(ISpatialObject* pObj, Node* pWhere);

	//non-recursive
	Node* SearchNodeUp(KEY P, Node* pNode) const;
	
	// recursive
	void DeleteTree(Node*& pNode);

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
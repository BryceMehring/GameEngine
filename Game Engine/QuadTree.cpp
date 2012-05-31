
#include "QuadTree.h"
#include "DxPolygon.h"
#include "IRenderer.h"
#include <algorithm>
#include <stack>

#define GET_INDEX(Node1) (Node1->m_Previous == nullptr) ? -1l : (MAX_NODES - (Node1->m_Previous->m_Nodes[MAX_NODES - 1] - Node1))

using namespace std;

unsigned int LOG2(unsigned int v)
{
	const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const unsigned int S[] = {1, 2, 4, 8, 16};
	
	unsigned int r = 0; // result of log2(v) will go here
	for (int i = 4; i >= 0; i--) // unroll for speed...
	{
		if (v & b[i])
		{
			v >>= S[i];
			r |= S[i];
		} 
	}

	return r;
}

bool operator == (const POINT& a, const POINT& b)
{
	return ((a.x == b.x) && (a.y == b.y));
}
bool operator != (const POINT& a, const POINT& b)
{
	return !::operator==(a,b);
}

Node::Node() : m_Previous(nullptr), m_pObjects(nullptr)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
}

Node::Node(const FRECT& R) : m_Previous(nullptr), m_pObjects(nullptr), R(R)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
	SubDivide();
}
Node::~Node()
{
	// If m_pObjects points to an object
	if(m_pObjects != nullptr)
	{
		// Notify each object in the node about deletion
		for_each(m_pObjects->begin(),m_pObjects->end(),[](ISpatialObject* pObj)
		{
			pObj->ClearNodes();
		});

		// The delete operator already checks if m_pObjects is null, but because of the 
		// for_each loop, this is needed.
		delete m_pObjects; 
		m_pObjects = nullptr;

		delete[] m_Nodes[0];
	}
}

void Node::SetRect(const CRectangle& R)
{
	this->R = R;
}
bool Node::IsWithin(const ISpatialObject* pObj) const
{
	return pObj->GetCollisionPolygon()->Intersects(&R);
}


bool Node::IsDivided() const
{
	return (m_Nodes[0] != nullptr);
}

bool Node::HasPoint() const
{
	bool success = false;

	if(m_pObjects != nullptr)
	{
		success = !m_pObjects->empty();
	}

	return success;
}

bool Node::IsFull() const
{
	return (m_pObjects->size()) >= 4;
}


void Node::SubDivide()
{
	//const GetRect& this->m
	const FRECT& rect = R.GetRect();

	// this will divide the current rect into MAX_NODES new rectangles
	D3DXVECTOR2 middle = rect.Middle();
	const D3DXVECTOR2& topLeft = rect.topLeft;
	const D3DXVECTOR2& bottomRight = rect.bottomRight;

	Node* pNodeArray = new Node[MAX_NODES];
	FRECT subRects[MAX_NODES] =
	{
		FRECT(topLeft,middle),
		FRECT(D3DXVECTOR2(middle.x,topLeft.y),D3DXVECTOR2(bottomRight.x,middle.y)),
		FRECT(D3DXVECTOR2(topLeft.x,middle.y),D3DXVECTOR2(middle.x,bottomRight.y)),
		FRECT(middle,bottomRight)
		//{xMid,yMid,R.right,R.bottom}*/
	};

	// Loop over all rects, and create them
	for(unsigned int i = 0; i < MAX_NODES; ++i)
	{
		Node* pSubNode = m_Nodes[i] = pNodeArray + i;
		pSubNode->SetRect(subRects[i]);
		pSubNode->m_pObjects = new LIST_DTYPE();
		pSubNode->m_Previous = this;

		// If the current node has points, subdivide them
		if(m_pObjects)
		{
			LIST_DTYPE::iterator iter = m_pObjects->begin();
			for(; iter != m_pObjects->end(); ++iter)
			{
				if(pSubNode->IsWithin(*iter))
				{
					pSubNode->m_pObjects->insert(*iter);
					(*iter)->AddNode(pSubNode);
				}
			}
		}
	}

	// free memory, this node will no longer store points.
	delete m_pObjects;
	m_pObjects = nullptr;
}

void Node::Render(IRenderer* pRenderer)
{
	DxSquare square;

	for(NodeIterator iter = this; (*iter) != nullptr; ++iter)
	{
		if(iter->HasPoint())
		{
			RECT R = iter->R.GetRect().Rect();
			square.ConstructFromRect(R);
			square.Render(pRenderer);
		}
	}
}

bool Node::Insert(ISpatialObject* pObj)
{
	// If the point is within the the root
	if(IsWithin(pObj))
	{
		RInsert(pObj);
	}

	return true;
}

void Node::RInsert(ISpatialObject* pObj)
{
	// find the near nodes to pObj
	std::vector<Node*> nodes;
	FindNearNodes(pObj,nodes);

	// as we iterate over the near nodes
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		// if the current node is full
		if(nodes[i]->IsFull())
		{
			// subdivide the node
			nodes[i]->SubDivide();

			// Try to insert pObj into this sub node
			nodes[i]->Insert(pObj);
		}
		// node is not yet full
		else
		{
			// Add pObj to node
			nodes[i]->m_pObjects->insert(pObj);

			// Add node to pObj
			pObj->AddNode(nodes[i]);
		}
	}
}

// this method is recursive, for simplicity
void Node::FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out)
{
	if(IsDivided())
	{
		// Loop through all of the sub nodes
		for(unsigned int i = 0; i < MAX_NODES; ++i)
		{
			Node* pSubNode = m_Nodes[i];

			if(pSubNode->IsWithin(pObj)) // If the point is within the sub node  
			{
				// find the near nodes from this node
				pSubNode->FindNearNodes(pObj,out);
			}
		}
	}
	else
	{
		// At the bottom of the tree, pObj collides with the node
		out.push_back(this);
	}
}

void Node::Update()
{
	for(NodeIterator iter = this; iter != nullptr; ++iter)
	{
		if((*iter)->HasPoint())
		{
			Update(*iter);
		}
	}
}

void Node::Update(Node* pNode)
{
	Node::LIST_DTYPE::iterator iter = pNode->m_pObjects->begin();
	Node::LIST_DTYPE::iterator end = pNode->m_pObjects->end();
	while(iter != end)
	{
		std::vector<Node*> nodes;
		FindNearNodes(*iter,nodes);

		bool bErase = true;

		if(nodes.empty())
		{
			//bErase = true;
			iter = pNode->m_pObjects->erase(iter);
			continue;
		}

		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			bErase = nodes[i] != pNode;
			if(nodes[i] != pNode)
			{
				nodes[i]->Insert(*iter);
				//bErase = true;
			}
		}

		if(bErase)
		{
			//(*iter)->ClearNodes();
			iter = pNode->m_pObjects->erase(iter);
		}
		else
		{
			++iter;
		}

	}
}

void Node::ExpandLeft()
{
	// todo: need to implement
}

NodeIterator::NodeIterator(Node* pNode) : m_pNode(pNode) {}
NodeIterator& NodeIterator::operator++()
{
	Increment();
	return *this;
}
NodeIterator& NodeIterator::operator++(int unused)
{
	return this->operator++();
}
Node* NodeIterator::operator*()
{
	return m_pNode;
}
Node* NodeIterator::operator->()
{
	return this->operator*();
}

NodeIterator& NodeIterator::operator=(Node* pNode)
{
	m_pNode = pNode;
	return *this;
}

bool NodeIterator::operator==(Node* pNode)
{
	return m_pNode == pNode;
}

bool NodeIterator::operator!=(Node* pNode)
{
	return !this->operator==(pNode);
}

bool NodeIterator::operator==(const NodeIterator& node)
{
	return m_pNode == node.m_pNode;
}

bool NodeIterator::operator!=(const NodeIterator& node)
{
	return !this->operator==(node);
}

unsigned int NodeIterator::GetIndex() const
{
	return GET_INDEX(m_pNode);
	//return index;
}

void NodeIterator::LoopDown(unsigned int index)
{
	// Loop down
	m_pNode = m_pNode->m_Nodes[index];
}
void NodeIterator::LoopUp()
{
	// Loop up
	unsigned int index = GetIndex();

	m_pNode = m_pNode->m_Previous;

	if(index >= MAX_NODES)
	{
		do
		{
			index = GetIndex();
			m_pNode = m_pNode->m_Previous;
					
		} while(index == MAX_NODES);

		if(index == -1) return;				
	}			

	LoopDown(index);
}

void NodeIterator::Increment()
{
	if(m_pNode != nullptr)
	{
		if(m_pNode->IsDivided())
		{
			LoopDown(0);
		}
		else
		{
			LoopUp();
		}
	}
}

// constructor
QuadTree::QuadTree(const FRECT& R) : m_iDepth(0)
{
	m_pRoot = new Node(R);
	CalculateMaxSubDivisions();
}

// Destructor
QuadTree::~QuadTree()
{
	delete m_pRoot;
	m_pRoot = nullptr;
}

void QuadTree::CalculateMaxSubDivisions()
{
	//unsigned int logH = LOG2(m_pRoot->R.GetRect(). - m_pRoot->R.top);
	//unsigned int logW = LOG2(m_pRoot->R.right - m_pRoot->R.left);

	//m_iMaxSubDivisions = (logW < logH ? logW : logH) / 2;
	m_iMaxSubDivisions = 4;
}

bool QuadTree::Insert(ISpatialObject* pObj)
{
	return m_pRoot->Insert(pObj);
}

void QuadTree::Erase(ISpatialObject* pObj)
{
	std::vector<Node*> nodes;
	m_pRoot->FindNearNodes(pObj,nodes);
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		nodes[i]->Erase(pObj);
	}
}

void QuadTree::Update()
{
	m_pRoot->Update();
	//std::vector<Node*> emptyVector;

	// Loop through all the nodes
	/*NodeIterator iter = m_pRoot;
	while((*iter) != nullptr)
	{
		bool bTest = true;

		Node* pNode = *iter;

		if(pNode->HasPoint())
		{
			Update(pNode);

			/*if(!pNode->HasPoint())
			{
				if(pNode->m_Previous != nullptr && pNode->m_Previous->m_Previous != nullptr)
				{
					pNode->m_bDelete = true;
					emptyVector.push_back(pNode);
				}
			}
		}

		++iter;

		/*unsigned int index = GET_INDEX(pNode);

		if(index == MAX_NODES && (pNode->m_Previous->m_Previous != nullptr))
		{
			int count = 0;

			for(unsigned int i = 0; i < MAX_NODES; ++i)
			{
				if(pNode->m_Previous->m_Nodes[i]->m_bDelete)
				{
					++count;
				}
			}

			if(count == MAX_NODES)
			{
				m_Empty.

				bTest = false;

				++iter;
					
 				//iter = pNode->m_Previous;
				CheckNodeForDeletion(pNode,0);
			}
		}

		if(bTest)
		{
			++iter;
		}*/
		/*if(pNode->m_pObjects)
		{
			unsigned int index = GET_INDEX(pNode);
			if((index == MAX_NODES) && (pNode->m_Previous != nullptr) && (pNode->m_Previous->m_Previous != nullptr))
			{
				bool bHasPoint = false;

				for(unsigned int i = 0; i < MAX_NODES; ++i)
				{
					if(pNode->m_Previous->m_Nodes[i]->HasPoint())
					{
						bHasPoint = true;
					}
				}

				if(!bHasPoint)
				{
					iter = pNode->m_Previous;
					
 					//iter = pNode->m_Previous;
					CheckNodeForDeletion(pNode,0);
				}

			}
		}*/
	//}*/

	/*std::for_each(emptyVector.begin(),emptyVector.end(),[&](Node* pNode)
	{
		if(!pNode->IsDivided())
		{
			bool bHasNoPoint = true;

			for(unsigned int i = 0; i < MAX_NODES; ++i)
			{
				if(pNode->m_Previous->m_Nodes[i]->HasPoint())
				{
					bHasNoPoint = false;
				}
			}

			if(bHasNoPoint)
			{
				//CheckNodeForDeletion(pNode,0);
			}
		}
		//emptyVector[i]->m_Previous->m_Nodes
	});*/
}

/*void QuadTree::CheckNodeForDeletion(Node* pNode, int n)
{
	Node** pArray = pNode->m_Previous->m_Nodes;

	pNode->m_Previous->m_pObjects = nullptr;

	delete[] pArray[0];
	memset(pArray,0,sizeof(Node*)*MAX_NODES);

	// todo: check this function
	/*if(n == (MAX_NODES - 1))
	{
		bool bDelete = false;
		for(int i = 0; i < MAX_NODES; ++i)
		{
			if(pNode->m_Nodes[i]->HasPoint())
			{
				bDelete = false;
				break;
			}
		}

		if(bDelete)
		{
			delete[] pNode->m_Nodes[0];
			memset(pNode->m_Nodes,0,sizeof(Node*)*MAX_NODES);
		}
	}
}*/

void QuadTree::Update(Node* pNode)
{
	/*Node::LIST_DTYPE::iterator iter = pNode->m_pObjects->begin();
	Node::LIST_DTYPE::iterator end = pNode->m_pObjects->end();
	while(iter != end)
	{
		std::vector<Node*> nodes;
		FindNearNodes(*iter,nodes);

		bool bErase = nodes.empty();

		if(!bErase)
		{
			for(unsigned int i = 0; i < nodes.size(); ++i)
			{
				bErase = nodes[i] != pNode; 
				//if(bErase)
				{
					Insert(*iter,nodes[i]);
				}
			}
		}

		if(bErase)
		{
			//(*iter)->ClearNodes();
			iter = pNode->m_pObjects->erase(iter);
		}
		else
		{
			++iter;
		}

		/*if(!pNode->IsWithin(*iter))
		{
			// Search up to find which Node the point is in
			Node* pNewNode = SearchNodeUp((*iter),pNode);

			// If the point is within the root node
			if(pNewNode)
			{
				// add the point to the new node
				(Insert(*iter,pNewNode));
			}
			else
			{
				// the object left the area
				//(*iter)->SetNode(nullptr);
			}

			// erase the object from the list
			iter = pNode->m_pObjects->erase(iter);

			// todo: delete node
			/*if(pNode->m_pObjects->empty())
			{
				unsigned int index = GET_INDEX(pNode);

				if(index != -1)
				{
					pNode->m_Previous->m_Nodes[index];
				}
			}
		}
		else
		{
			++iter;
		}
	}*/
}

void QuadTree::SaveToFile(std::string& file)
{
	/*file.append(".quad");
	fstream fout(file.c_str(),ios::out);

	if(fout)
	{
		unsigned int counter = 0; 
		for(NodeIterator iter = m_pRoot; (*iter) != nullptr; ++iter)
		{
			fout << ++counter << " - " << iter->R.left << " " << iter->R.top << " " << iter->R.right << " " << iter->R.bottom ;
			
			fout  << " - \t{ ";

			if(iter->HasPoint())
			{
				// Loop over all points in the node
				std::list<KEY>::iterator listIter = iter->m_pObjects->begin(); 
				for(; listIter != iter->m_pObjects->end(); ++listIter)
				{
					fout << "(" << listIter->x << "," << listIter->y << "), ";
				}
			}

			fout << " }" << endl << endl; 
		}

		fout.close();

		system(file.c_str());
	}*/
}

void QuadTree::Render(IRenderer* pRenderer)
{
	m_pRoot->Render(pRenderer);
}




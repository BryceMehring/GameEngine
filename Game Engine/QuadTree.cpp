
#include "QuadTree.h"
#include "DxPolygon.h"
#include "IRenderer.h"
#include <algorithm>
#include <stack>

#define GET_INDEX(Node1) (Node1->m_Previous == nullptr) ? -1l : (MAX_NODES - (Node1->m_Previous->m_Nodes[MAX_NODES - 1] - Node1))

using namespace std;

bool operator == (const POINT& a, const POINT& b)
{
	return ((a.x == b.x) && (a.y == b.y));
}
bool operator != (const POINT& a, const POINT& b)
{
	return !::operator==(a,b);
}

Node::Node() : m_Previous(nullptr), m_pObjects(nullptr), m_bUseable(false)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
}

Node::Node(const Math::FRECT& R) : m_Previous(nullptr), m_pObjects(nullptr), R(R), m_bUseable(false)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
	SubDivide(true);
}
Node::~Node()
{
	// If m_pObjects points to an object
	if(m_pObjects != nullptr)
	{
		// The delete operator already checks if m_pObjects is null, but because of the 
		// for_each loop, this is needed.
		delete m_pObjects; 
		m_pObjects = nullptr;

		delete[] m_Nodes[0];
	}
}


void Node::SetRect(const Math::CRectangle& R)
{
	this->R = R;
}
bool Node::IsWithin(ISpatialObject& obj) const
{
	return R.Intersects(obj.GetCollisionPolygon());
}


bool Node::IsDivided() const
{
	return (m_Nodes[0] != nullptr);
}

bool Node::HasPoint() const
{
	return (m_pObjects != nullptr) && (!m_pObjects->empty());
}

bool Node::RHasPoint() const
{
	if(IsDivided())
	{
		for(unsigned int i = 0; i < MAX_NODES; ++i)
		{
			Node* pSubNode = m_Nodes[i];

			if(pSubNode->HasPoint())
			{
				return true;
			}
			else
			{
				return pSubNode->RHasPoint();
			}
		}
	}

	return HasPoint();
}

bool Node::IsFull() const
{
	return (m_pObjects->size()) >= 2;
}


void Node::SubDivide(bool bAlloc)
{
	//const GetRect& this->m
	const Math::FRECT& rect = R.GetRect();

	// this will divide the current rect into MAX_NODES new rectangles
	D3DXVECTOR2 middle = rect.Middle();
	const D3DXVECTOR2& topLeft = rect.topLeft;
	const D3DXVECTOR2& bottomRight = rect.bottomRight;

	Node* pNodeArray = new Node[MAX_NODES];
	Math::FRECT subRects[MAX_NODES] =
	{
		Math::FRECT(topLeft,middle),
		Math::FRECT(D3DXVECTOR2(middle.x,topLeft.y),D3DXVECTOR2(bottomRight.x,middle.y)),
		Math::FRECT(D3DXVECTOR2(topLeft.x,middle.y),D3DXVECTOR2(middle.x,bottomRight.y)),
		Math::FRECT(middle,bottomRight)
		//{xMid,yMid,R.right,R.bottom}*/
	};

	// Loop over all rects, and create them
	for(unsigned int i = 0; i < MAX_NODES; ++i)
	{
		Node* pSubNode = m_Nodes[i] = pNodeArray + i;
		pSubNode->SetRect(subRects[i]);
		pSubNode->m_pObjects = new LIST_DTYPE();
		pSubNode->m_Previous = this;
		pSubNode->m_bUseable = true;

		SubDivideObjects(*pSubNode);
	}

	m_bUseable = false;

	if(m_pObjects)
	{
		m_pObjects->clear();
	}

	// free memory, this node will no longer store points.
	//delete m_pObjects;
	//m_pObjects = nullptr;
}

void Node::SubDivideObjects(Node& subNode)
{
	if(m_pObjects)
	{
		LIST_DTYPE::iterator iter = m_pObjects->begin();
		LIST_DTYPE::iterator end = m_pObjects->end();
		for(; iter != end; ++iter)
		{
			if(subNode.IsWithin(**iter))
			{
				subNode.m_pObjects->insert(*iter);
			}
		}
	}
}

void Node::Render(IRenderer& renderer)
{
	DxSquare square;

	for(NodeIterator iter = this; (*iter) != nullptr; ++iter)
	{
		if(iter->HasPoint())
		{
			RECT R = iter->R.GetRect().Rect();
			square.ConstructFromRect(R);
			square.Render(renderer);
		}
	}
}

void Node::Erase(ISpatialObject& obj)
{
	auto iter = m_pObjects->find(&obj);
	if(iter != m_pObjects->end())
	{
		m_pObjects->erase(iter);
	}

		/*if(m_pObjects->empty() && (m_Previous->m_Previous != nullptr) && (m_Previous->m_bUseable == false))
		{
			bool success = false;

			for(unsigned int i = 0; i < MAX_NODES; ++i)
			{
				Node* pNode = m_Previous->m_Nodes[i];
				success |= pNode->RHasPoint();
			}
							
			if(!success)
			{
				m_Previous->m_bUseable = true;
				m_bUseable = false;

				for(unsigned int i = 0; i < MAX_NODES; ++i)
				{
					Node* pNode = m_Previous->m_Nodes[i];
					pNode->m_bUseable = false;
				}
			}
		}*/
	
}

void Node::EraseFromPreviousPos(ISpatialObject& obj)
{
	std::vector<Node*>& nodes = obj.m_nodes;

	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		nodes[i]->Erase(obj);
	}
}

bool Node::Insert(ISpatialObject& obj)
{
	// If the point is within the the root
	if(IsWithin(obj))
	{
		RInsert(obj);
	}

	return true;
}

void Node::RInsert(ISpatialObject& obj)
{
	// find the near nodes to pObj
	std::vector<Node*>& nodes = obj.m_nodes;
	nodes.clear();

	FindNearNodes(obj.GetCollisionPolygon(),nodes);

	// as we iterate over the near nodes
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		Node* pNode = nodes[i];

		const Math::FRECT& subR = nodes[i]->R.GetRect();

		// if the current node is full
		// 200
		// 400
		if((pNode->IsFull()) && (subR.bottomRight.x - subR.topLeft.x) > 400.0f)
		{
			if(!pNode->IsDivided())
			{
				// subdivide the node
				pNode->SubDivide(true);
			}
			else if(m_pObjects != nullptr)
			{
				for(unsigned int j = 0; j < MAX_NODES; ++j)
				{
					Node* pSubNode = m_Nodes[j];
					pSubNode->m_bUseable = true;

					SubDivideObjects(*pSubNode);
				}

				m_bUseable = false;
				m_pObjects->clear();
			}

			// Try to insert pObj into this sub node
			pNode->RInsert(obj);
		}
		// node is not yet full
		else
		{
			// Add pObj to node
			pNode->m_pObjects->insert(&obj);
		}
	}
}

// this method is recursive, for simplicity
void Node::FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<Node*>& out)
{
	// recursive version
	if(!m_bUseable)
	{
		// Loop through all of the sub nodes
		for(unsigned int i = 0; i < MAX_NODES; ++i)
		{
			Node* pSubNode = m_Nodes[i];

			// todo: this is bugged, sometimes m_bUseable is false when we are at the bottom, 
			// m_bUseable in this case should be true...

			if(pSubNode->R.Intersects(poly))
			{
				// find the near nodes from this node
				pSubNode->FindNearNodes(poly,out);
			}
		}
	}
	else
	{
		// At the bottom of the tree, pObj collides with the node
		out.push_back(this);
	}
	
	// non recursive version
	/*std::stack<Node*> theStack;
	theStack.push(this);

	do
	{
		Node* pTop = theStack.top();
		theStack.pop();

		if(pTop->IsDivided())
		{
			// Loop through all of the sub nodes
			for(unsigned int i = 0; i < MAX_NODES; ++i)
			{
				Node* pSubNode = pTop->m_Nodes[i];

				if(pSubNode->R.Intersects(pPolygon))  
				{
					theStack.push(pSubNode);
					// find the near nodes from this node
					//pSubNode->FindNearNodes(pObj,out);
				}
			}
		}
		else
		{
			out.push_back(pTop);
		}

	} while(!theStack.empty());*/
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

void ISpatialObject::EraseFromQuadtree(QuadTree* pTree)
{
	//pTree->EraseFromPrev(this);
	pTree->Erase(*this);
}

// constructor
QuadTree::QuadTree(const Math::FRECT& R)
{
	m_pRoot = new Node(R);
}

// Destructor
QuadTree::~QuadTree()
{
	delete m_pRoot;
	m_pRoot = nullptr;
}

bool QuadTree::IsWithin(ISpatialObject& obj) const
{
	return m_pRoot->IsWithin(obj);
}

bool QuadTree::Insert(ISpatialObject& obj)
{
	return m_pRoot->Insert(obj);
}

void QuadTree::Erase(ISpatialObject& obj)
{
	std::vector<Node*> nodes;
	m_pRoot->FindNearNodes(obj.GetCollisionPolygon(),nodes);
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		nodes[i]->Erase(obj);
	}
}

void QuadTree::EraseFromPrev(ISpatialObject& obj)
{
	m_pRoot->EraseFromPreviousPos(obj);
}

void QuadTree::FindNearObjects(Math::ICollisionPolygon* pPoly, std::vector<ISpatialObject*>& out)
{
	std::vector<Node*> nodes;
	FindNearNodes(pPoly,nodes);

	ProccessNearNodes(nodes,[&](ISpatialObject* pObj) -> bool
	{
		out.push_back(pObj);
		return false;
	});
	
}

void QuadTree::FindNearNodes(Math::ICollisionPolygon* pPoly, std::vector<Node*>& out)
{
	m_pRoot->FindNearNodes(*pPoly,out);
}

void QuadTree::FindNearNodes(ISpatialObject* pObj, std::vector<Node*>& out)
{
	m_pRoot->FindNearNodes(pObj->GetCollisionPolygon(),out);
}

void QuadTree::Update(ISpatialObject& obj)
{
	m_pRoot->EraseFromPreviousPos(obj);
	Insert(obj);
	//pObj->
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

void QuadTree::Render(IRenderer& renderer)
{
	m_pRoot->Render(renderer);
}





#include "QuadTree.h"
#include "DxPolygon.h"
#include <algorithm>

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

Node::Node() : m_Previous(nullptr), m_pKeys(nullptr)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
}

Node::Node(const RECT& R) : m_Previous(nullptr), m_pKeys(nullptr)
{
	AssignRect(R);
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
}
Node::~Node()
{
	// todo: destroy allocated memory
	delete m_pKeys;
	m_pKeys = nullptr;
}

void Node::SetRect(const RECT& R)
{
	AssignRect(R);
}

bool Node::IsPointWithin(KEY P) const
{
	return (PtInRect(&R,P) > 0);
}

bool Node::IsPointWithin(const ISpatialObject* pObj) const
{
	return IsPointWithin(pObj->GetPos());
}


bool Node::IsDivided() const
{
	return m_Nodes[0] != nullptr;
}

bool Node::HasPoint() const
{
	bool success = false;

	if(m_pKeys)
	{
		success = !m_pKeys->empty();
	}

	return success;
}

bool Node::IsFull() const
{
	return (m_pKeys->size()) >= 1;
}

void Node::AssignRect(const RECT& R)
{
	this->R = R;
}


void Node::SubDivide()
{
	// this will divide the current rect into MAX_NODES new rectangles
	// todo: need to organize function
	KEY P[9];

	int xMid = ((R.right - R.left) / 2);
	int yMid = ((R.bottom - R.top) / 2);

	int i = 0;
	int r = 0;
	Node* pNodeArray = new Node[MAX_NODES];

	for(int y = R.top; y <= R.bottom; y += yMid)
	{
		for(int x = R.left; x <= R.right; x += xMid, ++i)
		{
			P[i].x = x;
			P[i].y = y;

			if((i > 3) && (i % 3))
			{
				RECT tempR;

				// Fill rect
				tempR.left = P[i-4].x;
				tempR.top = P[i-4].y;

				tempR.right = P[i].x;
				tempR.bottom = P[i].y;

				// pSubNode points to the correct sub node
				Node* pSubNode = m_Nodes[r] = pNodeArray + (r++);

				// Set the subnode's fields
				pSubNode->SetRect(tempR);
				pSubNode->m_pKeys = new LIST_DTYPE(); // Alloc a list of ISpatialObject* to store the objects
				pSubNode->m_Previous = this;

				// If the current node has points, subdivide them
				if(m_pKeys)
				{
					LIST_DTYPE::iterator iter = m_pKeys->begin();
					for(; iter != m_pKeys->end(); ++iter)
					{
						if(pSubNode->IsPointWithin(*iter))
						{
							pSubNode->m_pKeys->insert(*iter);
							(*iter)->SetNode(pSubNode);
						}
					}
				}
			}
		}
	}

	// free memory, this node will no longer store points.
	delete m_pKeys;
	m_pKeys = nullptr;
}

void Node::ExpandLeft()
{
	// todo: need to implement
}

NodeIterator::NodeIterator(Node* pNode) : m_pNode(pNode) {}
NodeIterator& NodeIterator::operator=(const Node* pNode)
{
	this->m_pNode = m_pNode;
	return *this;
}
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
	if(m_pNode->IsDivided())
	{
		LoopDown(0);
	}
	else
	{
		LoopUp();
	}
}
  
PointIterator::PointIterator(Node* pNode) : NodeIterator(pNode)
{
	// todo: need to fix
	if(!pNode->HasPoint())
	{
		NodeIterator::Increment();
	}
}

void PointIterator::Increment()
{
	do
	{
		NodeIterator::Increment();

	} while(!m_pNode->HasPoint());
}


// constructor
QuadTree::QuadTree(const RECT& R)
{
	m_pRoot = new Node(R);
	m_pRoot->SubDivide();

	CalculateMaxSubDivisions();
}

// Destructor
QuadTree::~QuadTree()
{
	DeleteTree(m_pRoot);

	delete m_pRoot;
	m_pRoot = nullptr;
}

void QuadTree::CalculateMaxSubDivisions()
{
	unsigned int logH = LOG2(m_pRoot->R.bottom - m_pRoot->R.top);
	unsigned int logW = LOG2(m_pRoot->R.right - m_pRoot->R.left);

	m_iMaxSubDivisions = (logW < logH ? logW : logH) / 2;
}

bool QuadTree::Insert(ISpatialObject* pObj)
{
	return Insert(pObj,m_pRoot);
}

bool QuadTree::Insert(ISpatialObject* pObj, Node* pWhere)
{
	// todo: need to optimize this function, line #295
	// fixed

	Node* pNode = pWhere;
	const KEY& P = pObj->GetPos();
	bool success = false;

	// If the point is within the the root
	if(m_pRoot->IsPointWithin(P))
	{
		success = true;

		do
		{
			// Find node to insert point into
			pNode = FindNearNode(P,pNode);

			// check if the point has already been inserted
			auto iter = find_if(pNode->m_pKeys->begin(),pNode->m_pKeys->end(),[&](const ISpatialObject* p) -> bool
			{
				return p->GetPos() == P;
			});

			// If not
			if(iter == pNode->m_pKeys->end())
			{
				// insert point

				// If the node is full, subdivide it
				if(pNode->IsFull())
				{
					pNode->SubDivide();
				}
			}
			else
			{
				success = false;
			}

		} while(pNode->IsDivided());

		if(success)
		{
			// Insert point into the current node's list
			auto iterPair = pNode->m_pKeys->insert(pObj);
			if(iterPair.second)
			{
				pObj->SetNode(pNode);
			}
			else
			{
				success = false;
			}
		}
	}

	return success;
}
void QuadTree::Erase(ISpatialObject* pObj)
{
	Node* pNode = FindNearNode(pObj);
	pNode->Erase(pObj);
}

Node* QuadTree::FindNearNode(const KEY& P, Node* pNode) const
{
	if(!m_pRoot->IsPointWithin(P))
		return nullptr;

	// Loop while the current node has sub nodes
	while(pNode->IsDivided())
	{
		// Loop through all of the sub nodes
		for(int i = 0; i < MAX_NODES; ++i)
		{
			Node* pSubNode = pNode->m_Nodes[i];
			if(pSubNode->IsPointWithin(P)) // If the point is within the sub node  
			{
				// iterate to this sub node 
				pNode = pSubNode;

				// and then start the process all over again
				break;
			}
		}
	}

	return pNode;
}

Node* QuadTree::FindNearNode(const KEY& P) const
{
	return FindNearNode(P,m_pRoot);
}

Node* QuadTree::FindNearNode(ISpatialObject* pObj) const
{
	return FindNearNode(pObj->GetPos(),m_pRoot);
}

void QuadTree::Update()
{
	// Loop through all the nodes
	for(NodeIterator iter = m_pRoot; (*iter) != nullptr; ++iter)
	{
		if(iter->HasPoint())
		{
			Update(*iter);
		}
	}
}

void QuadTree::Update(Node* pNode)
{
	Node::LIST_DTYPE::iterator iter = pNode->m_pKeys->begin();
	Node::LIST_DTYPE::iterator end = pNode->m_pKeys->end();
	while(iter != end)
	{
		if(!pNode->IsPointWithin(*iter))
		{
			// Search up to find which Node the point is in
			Node* pNewNode = SearchNodeUp((*iter)->GetPos(),pNode);

			// If the point is within the root node
			if(pNewNode)
			{
				// add the point to the new node
				Insert(*iter,pNewNode);
			}
			else
			{
				// the object left the area
				(*iter)->SetNode(nullptr);
			}

			// erase the object from the list
			iter = pNode->m_pKeys->erase(iter);

			// todo: delete node
			/*if(pNode->m_pKeys->empty())
			{
				unsigned int index = GET_INDEX(pNode);

				if(index != -1)
				{
					pNode->m_Previous->m_Nodes[index];
				}
			}*/
		}
		else
		{
			++iter;
		}
	}
}

Node* QuadTree::SearchNodeUp(KEY P, Node* pNode) const
{
	Node* pIter = nullptr;

	// quick solution check
	if(m_pRoot->IsPointWithin(P))
	{
		pIter = pNode;

		do
		{
			pIter = pIter->m_Previous;

		// Loop while 
		} while(!pIter->IsPointWithin(P));
	}
	
	return pIter;
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
				std::list<KEY>::iterator listIter = iter->m_pKeys->begin(); 
				for(; listIter != iter->m_pKeys->end(); ++listIter)
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

void QuadTree::DeleteTree(Node*& pNode)
{
	// Loop through all nodes
	if(pNode->IsDivided())
	{
		for(int i = 0; i < MAX_NODES; ++i)
		{
			DeleteTree(pNode->m_Nodes[i]);
		}

		delete[] pNode->m_Nodes[0];
	}

}

void QuadTree::Render(IRenderer* pRenderer)
{
	DxSquare square;
	// loop over all nodes and render them
	for(NodeIterator iter = m_pRoot; (*iter) != nullptr; ++iter)
	{
		square.ConstructFromRect(iter->R);
		square.Render(pRenderer);
	}

}




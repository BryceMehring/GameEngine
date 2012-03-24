
#include "QuadTree.h"


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
	return PtInRect(&R,P) > 0;
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
	return m_pKeys->size() > 3;
}

void Node::AssignRect(const RECT& R)
{
	this->R = R;
}


void Node::SubDivide()
{
	// this will divide the current rect into MAX_NODES new rectangles
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
				RECT R;

				// Fill rect
				R.left = P[i-4].x;
				R.top = P[i-4].y;

				R.right = P[i].x;
				R.bottom = P[i].y;

				// pSubNode points to the correct sub node
				Node* pSubNode = m_Nodes[r] = pNodeArray + (r++);

				// Alloc a list of ISpatialObject* to store the objects
				LIST_DTYPE* pList = new LIST_DTYPE();

				// Set the subnode's fields
				pSubNode->SetRect(R);
				pSubNode->m_pKeys = pList;
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

	delete this->m_pKeys;
	this->m_pKeys = nullptr;
}

void Node::ExpandLeft()
{
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
	// m_Previous is null
	unsigned int index = -1;
	if(m_pNode->m_Previous)
	{
		index = (MAX_NODES - (m_pNode->m_Previous->m_Nodes[MAX_NODES - 1] - m_pNode));
		//index = (MAX_NODES - (m_pNode->m_Previous->m_Nodes[MAX_NODES - 1] - m_pNode));
	}
		
	return index;
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


QuadTree::QuadTree(const RECT& R)
{
	m_pRoot = new Node(R);
	m_pRoot->SubDivide();

	CalculateMaxSubDivisions();
}

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

void QuadTree::Insert(ISpatialObject* pObj)
{
	Insert(pObj,m_pRoot);
}

void QuadTree::Insert(ISpatialObject* pObj, Node* pWhere)
{
	Node* pNode = pWhere;
	const KEY& P = pObj->GetPos();

	// If the point is within the the root
	if(m_pRoot->IsPointWithin(P))
	{
		do
		{
			// Find node to insert point into
			pNode = FindNearNode(pObj);

			// If the node is full, subdivide it
			if(pNode->IsFull())
			{
				pNode->SubDivide();
			}

		} while(pNode->IsDivided());

		// Insert point into the current node's list
		auto success = pNode->m_pKeys->insert(pObj);
		if(success.second)
		{
			pObj->SetNode(pNode);
		}
	}
}
void QuadTree::Erase(ISpatialObject* pObj)
{
	Node* pNode = FindNearNode(pObj);
	pNode->Erase(pObj);
}

Node* QuadTree::FindNearNode(const KEY& P) const
{
	if(!m_pRoot->IsPointWithin(P))
		return nullptr;

	Node* pNode = m_pRoot;

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

Node* QuadTree::FindNearNode(ISpatialObject* pObj) const
{
	return this->FindNearNode(pObj->GetPos());
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




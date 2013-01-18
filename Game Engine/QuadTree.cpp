
#include "QuadTree.h"
#include "DxPolygon.h"
#include "IRenderer.h"
#include <algorithm>
#include <stack>
#include <set>

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

Node::Node() : m_Previous(nullptr)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
}

Node::Node(const Math::FRECT& R) : m_Previous(nullptr), R(R)
{
	memset(m_Nodes,0,sizeof(Node*)*MAX_NODES);
	SubDivide();
}
Node::~Node()
{
	delete[] m_Nodes[0];
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
	return (!m_Objects.empty());
}

bool Node::IsFull() const
{
	return (m_Objects.size()) >= 4;
}


void Node::SubDivide()
{
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
	};

	// Loop over all rects, and create them
	for(unsigned int i = 0; i < MAX_NODES; ++i)
	{
		Node* pSubNode = m_Nodes[i] = pNodeArray + i;
		pSubNode->R = subRects[i];
		pSubNode->m_Previous = this;
	}
}

void Node::Render(IRenderer& renderer)
{
	for(NodeIterator iter = this; (*iter) != nullptr; ++iter)
	{
		if(iter->HasPoint())
		{
			const Math::FRECT& R = iter->GetRect();

			::D3DXVECTOR3 pos[5] = 
			{
				D3DXVECTOR3(R.topLeft.x,R.topLeft.y,0.0f),
				D3DXVECTOR3(R.bottomRight.x,R.topLeft.y,0.0f),
				D3DXVECTOR3(R.bottomRight.x,R.bottomRight.y,0.0f),
				D3DXVECTOR3(R.topLeft.x,R.bottomRight.y,0.0f),
				D3DXVECTOR3(R.topLeft.x,R.topLeft.y,0.0f),
			};

			renderer.Get2DRenderer().DrawLine(pos,5);
		}
	}
}

void Node::Erase(ISpatialObject& obj)
{
	std::vector<Node*> nodes;
	stack<Node*> theStack;

	theStack.push(this);

	while(!theStack.empty())
	{
		Node* pTop = theStack.top();
		theStack.pop();

		pTop->FindNearNodes(obj.GetCollisionPolygon(),nodes);

		// as we iterate over the near nodes
		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			Node* pNode = nodes[i];
			auto& objList = pNode->m_Objects;

			auto iter = std::find(objList.begin(),objList.end(),&obj);
			if(iter != objList.end())
			{
				objList.erase(iter);
			}

			theStack.push(pNode);
		}
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
	// non recursive version

	// the stack emulates recursion
	/*std::stack<Node*> callstack;
	callstack.push(this);

	// reference to vector of node pointers within the ISpatialObject
	std::vector<Node*>& nodes = obj.m_nodes;

	// Loop while 
	while(!callstack.empty())
	{
		Node* pTop = callstack.top();
		callstack.pop();

		nodes.clear();

		pTop->FindNearNodes(obj.GetCollisionPolygon(),nodes);

		// as we iterate over the near nodes
		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			Node* pNode = nodes[i];

			const Math::FRECT& subR = nodes[i]->R.GetRect();

			// if the current node is full
			// 200
			// 400
			if((pNode->IsFull()) && (subR.bottomRight.x - subR.topLeft.x) > 200.0f)
			{
				// subdivide the node
				pNode->SubDivide(true);
				callstack.push(pNode);
				
				/*else if(m_pObjects != nullptr)
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

				
				//pNode->RInsert(obj);
			}
			// node is not yet full
			else
			{
				// Add pObj to node
				pNode->m_pObjects->insert(&obj);
			}
		}
	}*/

	// find the near nodes to pObj

	std::vector<Node*> nodes;

	FindNearNodes(obj.GetCollisionPolygon(),nodes);

	// as we iterate over the near nodes
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		Node* pNode = nodes[i];

		const Math::FRECT& subR = nodes[i]->R.GetRect();

		// if the current node is full
		if((pNode->IsFull()) && subR.Height() > 5.0f)
		{
			if(!pNode->IsDivided())
			{
				pNode->SubDivide();
			}

			// recursive call
			pNode->RInsert(obj);
		}
		else
		{
			pNode->m_Objects.push_back(&obj);
		}
	}

}

// this method is recursive, for simplicity
void Node::FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<Node*>& out)
{
	out.clear();

	// recursive version
	if(IsDivided())
	{
		// Loop through all of the sub nodes
		for(unsigned int i = 0; i < MAX_NODES; ++i)
		{
			Node* pSubNode = m_Nodes[i];

			if(pSubNode->R.Intersects(poly))
			{
				out.push_back(pSubNode);
			}
		}
	}
	/*else
	{
		// At the bottom of the tree, pObj collides with the node
		out.push_back(this);
	}*/
	
	// non recursive version
	/*std::stack<Node*> theStack;
	theStack.push(this);

	while(!theStack.empty())
	{
		Node* pTop = theStack.top();
		theStack.pop();

		if(pTop->IsDivided())
		{
			// Loop through all of the sub nodes
			for(unsigned int i = 0; i < MAX_NODES; ++i)
			{
				Node* pSubNode = pTop->m_Nodes[i];

				if(pSubNode->R.Intersects(poly))
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

	} */
}

void Node::QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out)
{
	std::set<ISpatialObject*> onceFilter;
	std::vector<Node*> nodes;
	stack<Node*> theStack;

	theStack.push(this);

	while(!theStack.empty())
	{
		Node* pTop = theStack.top();
		theStack.pop();

		pTop->FindNearNodes(poly,nodes);

		// as we iterate over the near nodes
		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			Node* pNode = nodes[i];
			auto& objList = pNode->m_Objects;

			for(auto iter = objList.begin(); iter != objList.end(); ++iter)
			{
				if((*iter)->GetCollisionPolygon().Intersects(poly))
				{
					auto onceFilterIter = onceFilter.insert(*iter);

					if(onceFilterIter.second)
					{
						out.push_back(*iter);
					}
				}
			}

			theStack.push(pNode);
		}
	}
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
	m_pRoot->Erase(obj);
}

void QuadTree::FindNearObjects(const Math::ICollisionPolygon* pPoly, std::vector<ISpatialObject*>& out)
{
	m_pRoot->QueryNearObjects(*pPoly,out);
}

void QuadTree::FindNearNodes(const Math::ICollisionPolygon* pPoly, std::vector<Node*>& out)
{
	m_pRoot->FindNearNodes(*pPoly,out);
}

void QuadTree::FindNearNodes(const ISpatialObject* pObj, std::vector<Node*>& out)
{
	m_pRoot->FindNearNodes(pObj->GetCollisionPolygon(),out);
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

void QuadTree::Render(IRenderer& renderer)
{
	m_pRoot->Render(renderer);
}





#include "QuadTree.h"
#include "IRenderer.h"
#include <algorithm>
#include <stack>
#include <set>
#include <queue>
#include <fstream>

#define MAX_NODES 4
#define GET_INDEX(Node1) (Node1->m_Previous == nullptr) ? -1l : (MAX_NODES - (&Node1->m_Previous->m_Nodes[MAX_NODES - 1] - Node1))

using namespace std;

QuadTree::QuadTree() : m_Previous(nullptr), m_iHeight(0)
{
}

QuadTree::QuadTree(const Math::FRECT& R) : R(R), m_Previous(nullptr), m_iHeight(0)
{
	SubDivide();
}
QuadTree::~QuadTree()
{
}

bool QuadTree::IsWithin(ISpatialObject& obj) const
{
	return R.Intersects(obj.GetCollisionPolygon());
}


bool QuadTree::IsDivided() const
{
	return (!m_Nodes.empty());
}

bool QuadTree::HasPoint() const
{
	return (!m_Objects.empty());
}

bool QuadTree::IsFull() const
{
	return (m_Objects.size()) >= 3;
}


void QuadTree::SubDivide()
{
	const Math::FRECT& rect = R.GetRect();

	// this will divide the current rect into MAX_NODES new rectangles
	glm::vec2 middle = rect.Middle();
	const glm::vec2& topLeft = rect.topLeft;
	const glm::vec2& bottomRight = rect.bottomRight;

	m_Nodes.resize(4);

	Math::FRECT subRects[] =
	{
		Math::FRECT(topLeft,middle),
		Math::FRECT(glm::vec2(middle.x,topLeft.y),glm::vec2(bottomRight.x,middle.y)),
		Math::FRECT(glm::vec2(topLeft.x,middle.y),glm::vec2(middle.x,bottomRight.y)),
		Math::FRECT(middle,bottomRight)
	};

	// Loop over all rects, and create them
	for(unsigned int i = 0; i < m_Nodes.size(); ++i)
	{
		m_Nodes[i].R = subRects[i];
		m_Nodes[i].m_iHeight = m_iHeight + 1;
		m_Nodes[i].m_Previous = this;
	}
}

void QuadTree::Render(IRenderer& renderer)
{
	for(NodeIterator iter = this; (*iter) != nullptr; ++iter)
	{
		if(iter->HasPoint())
		{
			const Math::FRECT& R = iter->GetRect();

			::glm::vec3 pos[5] = 
			{
				glm::vec3(R.topLeft.x,R.topLeft.y,0.0f),
				glm::vec3(R.bottomRight.x,R.topLeft.y,0.0f),
				glm::vec3(R.bottomRight.x,R.bottomRight.y,0.0f),
				glm::vec3(R.topLeft.x,R.bottomRight.y,0.0f),
				glm::vec3(R.topLeft.x,R.topLeft.y,0.0f),
			};

			renderer.DrawLine(pos,5);
		}
	}
}

void QuadTree::Erase(ISpatialObject& obj)
{
	std::vector<QuadTree*> nodes;
	stack<QuadTree*> theStack;

	theStack.push(this);

	while(!theStack.empty())
	{
		QuadTree* pTop = theStack.top();
		theStack.pop();

		pTop->FindNearNodes(obj.GetCollisionPolygon(),nodes);

		// as we iterate over the near nodes
		for(unsigned int i = 0; i < nodes.size(); ++i)
		{
			QuadTree* pNode = nodes[i];
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

bool QuadTree::Insert(ISpatialObject& obj)
{
	// If the point is within the the root
	if(IsWithin(obj))
	{
		RInsert(obj);
	}

	return true;
}

void QuadTree::RInsert(ISpatialObject& obj)
{
	std::vector<QuadTree*> nodes;

	FindNearNodes(obj.GetCollisionPolygon(),nodes);

	// as we iterate over the near nodes
	for(unsigned int i = 0; i < nodes.size(); ++i)
	{
		QuadTree* pNode = nodes[i];

		const Math::FRECT& subR = nodes[i]->R.GetRect();

		// if the current node is full
		if((pNode->IsFull()) && subR.Height() > 10.0f)
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
void QuadTree::FindNearNodes(const Math::ICollisionPolygon& poly, std::vector<QuadTree*>& out)
{
	out.clear();

	// Loop through all of the sub nodes
	for(unsigned int i = 0; i < m_Nodes.size(); ++i)
	{
		QuadTree& subNode = m_Nodes[i];

		if(subNode.R.Intersects(poly))
		{
			out.push_back(&subNode);
		}
	}
}

void QuadTree::QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out, const ISpatialObject* pObj)
{
	//std::set<ISpatialObject*> onceFilter;
	stack<QuadTree*> theStack;

	theStack.push(this);

	while(!theStack.empty())
	{
		QuadTree* pTop = theStack.top();
		theStack.pop();

		// Loop through all of the sub nodes
		for(unsigned int i = 0; i < pTop->m_Nodes.size(); ++i)
		{
			QuadTree& subNode = pTop->m_Nodes[i];

			if(subNode.R.Intersects(poly))
			{
				auto& objList = subNode.m_Objects;

				for(auto iter = objList.begin(); iter != objList.end(); ++iter)
				{
					if(pObj == nullptr || (pObj != nullptr && (&pObj->GetCollisionPolygon()) != (&(*iter)->GetCollisionPolygon())))
					{
						if((*iter)->GetCollisionPolygon().Intersects(poly))
						{
							//auto onceFilterIter = onceFilter.insert(*iter);

							//if(onceFilterIter.second)
							{
								out.push_back(*iter);
							}
						}
					}
				}

				theStack.push(&subNode);
			}
		}
	}
}

NodeIterator::NodeIterator(QuadTree* pNode) : m_pNode(pNode) {}
NodeIterator& NodeIterator::operator++()
{
	Increment();
	return *this;
}
NodeIterator& NodeIterator::operator++(int unused)
{
	return this->operator++();
}
QuadTree* NodeIterator::operator*()
{
	return m_pNode;
}
QuadTree* NodeIterator::operator->()
{
	return this->operator*();
}

NodeIterator& NodeIterator::operator=(QuadTree* pNode)
{
	m_pNode = pNode;
	return *this;
}

bool NodeIterator::operator==(QuadTree* pNode)
{
	return m_pNode == pNode;
}

bool NodeIterator::operator!=(QuadTree* pNode)
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
	m_pNode = &m_pNode->m_Nodes[index];
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
	}

	// todo: fix this, it is bugged
	/*if(index != -1)
	{
		LoopDown(index);
	}*/
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
/*QuadTree::QuadTree(const Math::FRECT& R)
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
	file += ".quad"; 
	std::fstream fout(file.c_str(),ios::out);

	if(fout)
	{
		queue<Node*> verts;
		verts.push(m_pRoot);

		int iCurrentHeight = 0;

		while(!verts.empty())
		{
			Node* pFront = verts.front();
			verts.pop();

			if(pFront->HasPoint())
			{
				if(iCurrentHeight != pFront->m_iHeight)
				{
					fout<<endl<<pFront->m_iHeight<<":  ";
				}

				iCurrentHeight = pFront->m_iHeight;

				// write to file here
				fout<<pFront->m_Objects.size()<<",";
			}

			for(unsigned int i = 0; i < pFront->m_Nodes.size(); ++i)
			{
				verts.push(&pFront->m_Nodes[i]);
			}
		}

		fout.close();

		system(file.c_str());
	}
}

void QuadTree::Render(IRenderer& renderer)
{
	m_pRoot->Render(renderer);
}*/




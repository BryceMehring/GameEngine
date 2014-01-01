
#include "QuadTree.h"
#include "IRenderer.h"
#include "NodeIterator.h"
#include <algorithm>
#include <stack>
#include <set>
#include <queue>
#include <fstream>

using namespace std;

QuadTree::QuadTree(const Math::FRECT& R, unsigned int nodeCapacity) : m_Rect(R), m_Previous(nullptr), m_iCapacity(nodeCapacity), m_iHeight(0)
{
	SubDivide();
}

QuadTree::QuadTree(const Math::FRECT& rect, unsigned int nodeCapacity, unsigned int height, QuadTree* pPrevious) : 
	m_Rect(rect), m_Previous(pPrevious), m_iCapacity(nodeCapacity), m_iHeight(height)
{
}

bool QuadTree::Insert(ISpatialObject& obj)
{
	bool bSuccess = false;

	if (IsDivided())
	{
		const Math::ICollisionPolygon& collisionPoly = obj.GetCollisionPolygon();

		// iterate over the near nodes
		for (unsigned int i = 0; i < 4; ++i)
		{
			QuadTree& subNode = m_Nodes[i];

			if (subNode.m_Rect.Intersects(collisionPoly))
			{
				const Math::FRECT& subR = subNode.m_Rect.GetRect();

				// if the current node is full
				if (subNode.IsFull() && subR.Height() > 4.0f)
				{
					if (!subNode.IsDivided())
					{
						subNode.SubDivide();
					}
				}
			}

			bSuccess |= subNode.Insert(obj);
		}
	}
	else
	{
		m_Objects.push_back(&obj);
		bSuccess = true;
	}

	return bSuccess;

}

void QuadTree::Erase(ISpatialObject& obj)
{
	if (IsDivided())
	{
		const Math::ICollisionPolygon& collisionPoly = obj.GetCollisionPolygon();

		// Loop through all of the sub nodes
		for (unsigned int i = 0; i < m_Nodes.size(); ++i)
		{
			QuadTree& subNode = m_Nodes[i];

			if (subNode.m_Rect.Intersects(collisionPoly))
			{
				subNode.Erase(obj);
			}
		}
	}
	else
	{
		auto iter = std::find(m_Objects.begin(), m_Objects.end(), &obj);

		if (iter != m_Objects.end())
		{
			m_Objects.erase(iter);
		}
	}
}

void QuadTree::QueryNearObjects(const ISpatialObject* pObj, std::vector<ISpatialObject*>& out)
{
	QueryNearObjects(pObj->GetCollisionPolygon(), out, pObj);
}

void QuadTree::QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out)
{
	QueryNearObjects(poly, out, nullptr);
}

bool QuadTree::IsWithin(ISpatialObject& obj) const
{
	return m_Rect.Intersects(obj.GetCollisionPolygon());
}

const Math::FRECT& QuadTree::GetRect() const
{
	return m_Rect.GetRect();
}

void QuadTree::Render(IRenderer& renderer)
{
	for (NodeIterator iter = this; (*iter) != nullptr; ++iter)
	{
		if(iter->HasObjects())
		{
			const Math::FRECT& R = iter->GetRect();

			::glm::vec3 pos[5] =
			{
				glm::vec3(R.topLeft.x, R.topLeft.y, -20.0f),
				glm::vec3(R.bottomRight.x, R.topLeft.y, -20.0f),
				glm::vec3(R.bottomRight.x, R.bottomRight.y, -20.0f),
				glm::vec3(R.topLeft.x, R.bottomRight.y, -20.0f),
				glm::vec3(R.topLeft.x, R.topLeft.y, -20.0f),
			};

			renderer.DrawLine(pos, 5, 20.0f);
		}
	}
}

bool QuadTree::IsDivided() const
{
	return (!m_Nodes.empty());
}

bool QuadTree::HasObjects() const
{
	return (!m_Objects.empty());
}

bool QuadTree::IsFull() const
{
	return (m_Objects.size() >= m_iCapacity);
}


void QuadTree::SubDivide()
{
	const Math::FRECT& rect = m_Rect.GetRect();

	glm::vec2 middle = rect.Middle();
	const glm::vec2& topLeft = rect.topLeft;
	const glm::vec2& bottomRight = rect.bottomRight;

	Math::FRECT subRects[] =
	{
		Math::FRECT(topLeft,middle),
		Math::FRECT(glm::vec2(middle.x,topLeft.y),glm::vec2(bottomRight.x,middle.y)),
		Math::FRECT(glm::vec2(topLeft.x,middle.y),glm::vec2(middle.x,bottomRight.y)),
		Math::FRECT(middle,bottomRight)
	};

	m_Nodes.reserve(4);

	// Loop over all sub-quadrants, and create them
	for(unsigned int i = 0; i < 4; ++i)
	{
		m_Nodes.push_back(QuadTree(subRects[i],m_iCapacity,m_iHeight + 1,this));

		for(auto& iter : m_Objects)
		{
			m_Nodes[i].Insert(*iter);
		}
	}

	m_Objects.clear();
}

void QuadTree::QueryNearObjects(const Math::ICollisionPolygon& poly, std::vector<ISpatialObject*>& out, const ISpatialObject* pObj)
{
	std::set<ISpatialObject*> onceFilter;
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

			if(subNode.m_Rect.Intersects(poly))
			{
				if(!subNode.IsDivided())
				{
					auto& objList = subNode.m_Objects;

					for(auto iter = objList.begin(); iter != objList.end(); ++iter)
					{
						if(pObj == nullptr || (pObj != nullptr && (&pObj->GetCollisionPolygon()) != (&(*iter)->GetCollisionPolygon())))
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
					}
				}
				else
				{
					theStack.push(&subNode);
				}
			}
		}
	}
}





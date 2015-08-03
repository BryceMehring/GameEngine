#include "Node.h"
#include <limits>

Node::Node(const glm::vec2& pos) : m_pos(pos), m_cost(std::numeric_limits<float>::max()), m_pPredecessor(nullptr), m_pLowestEdge(nullptr)
{
}

void Node::AddEdge(Edge* pEdge)
{
	if (pEdge != nullptr)
	{
		m_edges.push_back(pEdge);
	}
}

const std::vector<Edge*>& Node::GetEdges() const
{
	return m_edges;
}

void Node::SetPos(const glm::vec2& pos)
{
	m_pos = pos;
}

glm::vec2 Node::GetPos() const
{
	return m_pos;
}

void Node::SetCost(float c)
{
	m_cost = c;
}
float Node::GetCost() const
{
	return m_cost;
}

void Node::SetPredecessor(Node* pNode)
{
	m_pPredecessor = pNode;
}
Node* Node::GetPredecessor() const
{
	return m_pPredecessor;
}

void Node::SetLowestEdge(Edge* pEdge)
{
	m_pLowestEdge = pEdge;
}

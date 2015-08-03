#include "Edge.h"

Edge* Node::GetLowestEdge() const
{
	return m_pLowestEdge;
}

Edge::Edge(Node* pHead, Node* pTail, float length) : m_pHead(pHead), m_pTail(pTail), m_fCost(length)
{
}

Node* Edge::GetHead()
{
	return m_pHead;
}
Node* Edge::GetTail()
{
	return m_pTail;
}

float Edge::GetCost() const
{
	return m_fCost;
}

#ifndef EDGE_H
#define EDGE_H

#include "Node.h"

// Defines an edge which links two nodes with cost
class Edge
{
public:

	Edge(Node* pHead, Node* pTail, float cost);

	Node* GetHead();
	Node* GetTail();

	// Returns the cost of the edge
	float GetCost() const;

private:

	Node* m_pHead;
	Node* m_pTail;
	float m_fCost;
};

#endif // EDGE_H

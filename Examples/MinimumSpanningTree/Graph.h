#ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include "Edge.h"

#include <vector>
#include <memory>

// Node Comparison functor
class NodeGreaterThan
{
public:

	bool operator()(const Node* a, const Node* b) const
	{
		return a->GetCost() > b->GetCost();
	}
};

// Defines a graph which supports generation of the Minimum spanning tree
class Graph
{
public:

	// Adds pNode to the graph
	void AddNode(Node* pNode);

	const std::vector<std::unique_ptr<Node>>& GetNodes() const;

	// Creates an edge from pHead to pTail with a weight. Set bTwoWay to true if the edge should point in both directions
	void CreateEdge(Node* pHead, Node* pTail, float cost, bool bTwoWay = true);

	// Enables the MST to be drawn if bDraw is true
	// else, the enitre graph is enabled to be drawn
	void EnableMST(bool bDraw);

	// Builds the Minimum spanning tree from the graph
	void GenerateMST() const;

	// Renders the graph with weights
	// If MST is enabled via EnableMST(), then the MST is drawn
	// else, the entire graph is drawn
	void Render(class IRenderer& renderer) const;

private:

	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector<std::unique_ptr<Edge>> m_edges;
	bool m_bDrawMST = false;
};

#endif // GRAPH_H

#ifndef NODE_H
#define NODE_H

#include <glm/vec2.hpp>
#include <vector>

class Edge;

// Defines a 2d node which can be connected by edges
class Node
{
public:

	Node(const glm::vec2& pos);

	// Add an edge that connects with this node
	void AddEdge(Edge*);

	// Get a list of all edges that are connected to this node
	const std::vector<Edge*>& GetEdges() const;

	// Position of the Node
	void SetPos(const glm::vec2& pos);
	glm::vec2 GetPos() const;

	// Cost to proccess this node
	void SetCost(float c);
	float GetCost() const;

	// Predecessor used during the algorithm
	void SetPredecessor(Node* pNode);
	Node* GetPredecessor() const;

	// Lowest edge found connecting to this node
	void SetLowestEdge(Edge* pEdge);
	Edge* GetLowestEdge() const;

private:
	glm::vec2 m_pos;
	std::vector<Edge*> m_edges;
	float m_cost;
	Node* m_pPredecessor;
	Edge* m_pLowestEdge;
};

#endif // NODE_H

#ifndef _MINIMUMSPANNINGTREE_
#define _MINIMUMSPANNINGTREE_

#include "IGameState.h"
#include "Camera.h"

#include <glm/vec2.hpp>
#include <vector>
#include <memory>

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

// Node Comparison functor
class NodeGreaterThan
{
public:

	bool operator()(const Node* a, const Node* b) const
	{
		return a->GetCost() > b->GetCost();
	}
};

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

// Defines a graph which supports generation of the Minimum spanning tree
class Graph
{
public:

	// Adds pNode to the graph
	void AddNode(Node* pNode);

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

// Minimum Spanning Tree base game plugin implementation
class MinimumSpanningTree : public IGameState
{
public:

	MinimumSpanningTree();
	~MinimumSpanningTree(); 

	// IPlugin

	// returns the type of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::Game; }

	// returns the name of the plug-in
	virtual const char* GetName() const { return "MinimumSpanningTree"; }

	// returns the version number of the plugin
	virtual int GetVersion() const { return 0; }

	// IGameState

	// Called only once when the plugin is created
	virtual void Init(class Game& game);

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game);

	// Called every frame to update the state of the game
	virtual void Update(class Game& game);

	// Called every frame to render the game
	virtual void Draw(class Game& game);

private:

	void BuildCamera(class Game& game);
	void BuildGraph(class Game& game);

	Graph* m_pGraph;
	PerspectiveCamera m_camera;
	bool m_bEnableMST;
	
};

#endif // _MINIMUMSPANNINGTREE_
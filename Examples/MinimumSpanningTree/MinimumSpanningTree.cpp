#include "MinimumSpanningTree.h"
#include "Game.h"
#include "PriorityQueue.h"
#include "Log.h"

#include <sstream>
#include <set>
#include <cmath>
#include <queue>
#include <cfloat>
#include <glm/geometric.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new MinimumSpanningTree();
}

Node::Node(const glm::vec2& pos) : m_pos(pos), m_cost(FLT_MAX), m_pPredecessor(nullptr), m_pLowestEdge(nullptr)
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

void Graph::AddNode(Node* pNode)
{
	if (pNode != nullptr)
	{
		m_nodes.emplace_back(pNode);
	}
}
void Graph::CreateEdge(Node* pHead, Node* pTail, float cost, bool bTwoWay)
{
	if (pHead == nullptr || pTail == nullptr)
		return;

	Edge* pEdge = new Edge(pHead, pTail, cost);

	pHead->AddEdge(pEdge);

	m_edges.emplace_back(pEdge);

	if (bTwoWay)
	{
		pEdge = new Edge(pTail, pHead, cost);

		pTail->AddEdge(pEdge);

		m_edges.emplace_back(pEdge);
	}
}

void Graph::EnableMST(bool bDraw)
{
	m_bDrawMST = bDraw;
}

void Graph::GenerateMST() const
{
	if (m_nodes.empty())
		return;

	cds::PriorityQueue<Node*, std::vector<Node*>, NodeGreaterThan> fringe;

	Node* pCurrentVertex = m_nodes[0].get();

	fringe.Push(pCurrentVertex);

	while (!fringe.Empty())
	{
		const std::vector<Edge*>& edges = pCurrentVertex->GetEdges();

		for (auto iter : edges)
		{
			Node* pNeighbor = iter->GetTail();

			if (pNeighbor != pCurrentVertex->GetPredecessor())
			{
				float fEdgeCost = iter->GetCost();
				float fCurrentCost = pNeighbor->GetCost();

				if (fEdgeCost < fCurrentCost)
				{
					pNeighbor->SetCost(fEdgeCost);
					pNeighbor->SetLowestEdge(iter);
					fringe.Push(pNeighbor);
				}
			}
		}
		
		if (!fringe.Empty())
		{
			pCurrentVertex = fringe.Top();
			if (pCurrentVertex->GetPredecessor() == nullptr && pCurrentVertex != m_nodes[0].get())
			{
				pCurrentVertex->SetPredecessor(pCurrentVertex->GetLowestEdge()->GetHead());
			}
			
			fringe.Pop();
		}
	} 

}

void Graph::Render(IRenderer& renderer) const
{
	auto renderCallback = [&](const glm::vec3 (&line)[2], float cost) -> void
	{
		std::stringstream stream;
		stream.precision(1);
		stream << std::fixed << cost;

		glm::vec3 middle = (line[0] + line[1]) / 2.0f;
		renderer.DrawString(stream.str().data(), middle, glm::vec4(1.0f), 30.0f, 0, FontAlignment::Center);

		renderer.DrawLine(line, 2, 3.0f, glm::vec4(0.5f*cos(cost / 10.0f) + 0.5f, 0.5f*sin(cost / 10.0f) + 0.5f, 0.8f, 1.0f));
	};

	for (auto& i : m_nodes)
	{
		glm::vec3 lineVerticies[2];
		float cost = 0.0f;
		if (m_bDrawMST)
		{
			Node* pNode = i->GetPredecessor();

			if (pNode != nullptr)
			{
				lineVerticies[0] = glm::vec3(pNode->GetPos(), 0.0f);
				lineVerticies[1] = glm::vec3(i->GetPos(), 0.0f);

				cost = i->GetLowestEdge()->GetCost();

				renderCallback(lineVerticies, cost);
			}
		}
		else
		{
			const std::vector<Edge*>& nearEdges = i->GetEdges();

			for (auto& j : nearEdges)
			{
				lineVerticies[0] = glm::vec3(j->GetHead()->GetPos(), 0.0f);
				lineVerticies[1] = glm::vec3(j->GetTail()->GetPos(), 0.0f);

				renderCallback(lineVerticies, j->GetCost());
			}
		}
	}
}

MinimumSpanningTree::MinimumSpanningTree() : m_pGraph(nullptr), m_bEnableMST(false)
{

}

MinimumSpanningTree::~MinimumSpanningTree()
{
	delete m_pGraph;
}

void MinimumSpanningTree::Init(Game& game)
{
	BuildCamera(game);
	BuildGraph(game);

	game.EnableEventWaiting(true);
}

void MinimumSpanningTree::Destroy(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	renderer.SetCamera(nullptr);
}

void MinimumSpanningTree::Update(Game& game)
{
	IInput& input = game.GetInput();
	if (input.KeyPress(GLFW_KEY_ENTER))
	{
		BuildGraph(game);
	}
	else if (input.KeyPress(GLFW_KEY_SPACE))
	{
		m_bEnableMST = !m_bEnableMST;

		m_pGraph->EnableMST(m_bEnableMST);
	}
}

void MinimumSpanningTree::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	int width, height;
	renderer.GetDisplayMode(&width, &height);

	// Render the graph in world space
	renderer.SetRenderSpace(RenderSpace::World);
	m_pGraph->Render(renderer);

	renderer.SetRenderSpace(RenderSpace::Screen);
	renderer.DrawString("spacebar: toggle drawing MST\tenter: generate a new MST", glm::vec3(width / 2.0f, height, -1.0f), glm::vec4(1.0f), 40.0f,nullptr,FontAlignment::Center);
}

void MinimumSpanningTree::BuildCamera(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	int width, height;

	// Build the world space camera
	renderer.GetDisplayMode(&width, &height);
	m_camera.SetLens(glm::pi<float>() / 2.0f, (float)width, (float)height, 0.0f, 50.0f);
	m_camera.LookAt(glm::vec3(0.0f, 0.0f, 200.0f));
	m_camera.Update();

	// Give our camera to the renderer
	renderer.SetCamera(&m_camera);
}

void MinimumSpanningTree::BuildGraph(Game& game)
{
	delete m_pGraph;
	m_pGraph = new Graph();

	Node* pNode = new Node(glm::vec2(-300,0.0f)); // left
	Node* pNode2 = new Node(glm::vec2(300.0f,0.0f)); // right
	Node* pNode3 = new Node(glm::vec2(0.0f)); // center
	Node* pNode4 = new Node(glm::vec2(0.0f,150.0f)); // up
	Node* pNode5 = new Node(glm::vec2(0.0f, -150.0f)); // down
	Node* pNode6 = new Node(glm::vec2(-150.0f,20.0f)); // left center
	Node* pNode7 = new Node(glm::vec2(150.0f, 20.0f)); // right center

	m_pGraph->AddNode(pNode);
	m_pGraph->AddNode(pNode2);
	m_pGraph->AddNode(pNode3);
	m_pGraph->AddNode(pNode4);
	m_pGraph->AddNode(pNode5);
	m_pGraph->AddNode(pNode6);
	m_pGraph->AddNode(pNode7);

	m_pGraph->CreateEdge(pNode, pNode6, glm::linearRand(0.0f,100.0f));
	m_pGraph->CreateEdge(pNode6, pNode3, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode6, pNode4, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode3, pNode7, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode7, pNode2, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode, pNode4, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode, pNode5, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode3, pNode4, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode2, pNode4, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode3, pNode5, glm::linearRand(0.0f, 100.0f));
	m_pGraph->CreateEdge(pNode2, pNode5, glm::linearRand(0.0f, 100.0f));

	m_pGraph->GenerateMST();
	m_pGraph->EnableMST(m_bEnableMST);
}

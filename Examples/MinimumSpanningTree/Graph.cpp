#include "Graph.h"
#include "PriorityQueue.h"
#include "IRenderer.h"

#include <sstream>

void Graph::AddNode(Node* pNode)
{
	if (pNode != nullptr)
	{
		m_nodes.emplace_back(pNode);
	}
}

const std::vector<std::unique_ptr<Node>>& Graph::GetNodes() const
{
	return m_nodes;
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

			if (pNeighbor->GetPredecessor() == nullptr)
			{
				float fEdgeCost = iter->GetCost();
				float fCurrentCost = pNeighbor->GetCost();

				if (fEdgeCost < fCurrentCost)
				{
					pNeighbor->SetLowestEdge(iter);
					pNeighbor->SetCost(fEdgeCost);
					fringe.Push(pNeighbor);
				}
			}
		}

		if (!fringe.Empty())
		{
			pCurrentVertex = fringe.Top();
			pCurrentVertex->SetPredecessor(pCurrentVertex->GetLowestEdge()->GetHead());
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

		renderer.DrawLine(line, 2, 3.0f, glm::vec4(cos(glm::radians(cost * 3.6f)) + 0.5f, sin(glm::radians(cost * 3.6f)) + 0.5f, 0.8f, 1.0f));
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

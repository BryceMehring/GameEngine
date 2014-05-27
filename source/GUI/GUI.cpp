#include "GUI.h"
#include <stack>
#include <set>

namespace UI
{
	unsigned int GUI::CreateNode()
	{
		m_matrix.emplace_back();

		for (auto& iter : m_matrix)
		{
			iter.resize(m_matrix.size());
		}

		m_nodes.emplace_back();

		return m_matrix.size() - 1;
	}

	void GUI::SetNode(unsigned int id)
	{
		if (id < m_matrix.size())
		{
			if ((m_uiCurrentNode == -1) || m_matrix[m_uiCurrentNode][id])
			{
				m_uiCurrentNode = id;
			}
		}
	}

	unsigned int GUI::GetNode() const
	{
		return m_uiCurrentNode;
	}

	void GUI::AddElement(unsigned int id, const std::shared_ptr<IElement>& pElement)
	{
		if (id < m_matrix.size())
		{
			m_nodes[id].elements.push_back(pElement);
		}
	}

	void GUI::LinkNodes(unsigned int id, unsigned int id2)
	{
		if (id < m_matrix.size() && id2 < m_matrix.size())
		{
			m_matrix[id][id2] = m_matrix[id2][id] = true;
		}
	}

	bool GUI::IsConnected() const
	{
		if (m_uiCurrentNode == -1)
			return false;

		// Use depth first search to find the connected components of the GUI graph.
		
		std::stack<int> nodes;
		std::set<int> closedList;
		nodes.push(m_uiCurrentNode);

		int connectedComponents = 0;
		while (!nodes.empty())
		{
			int index = nodes.top();
			nodes.pop();

			auto pair = closedList.insert(index);
			if (pair.second == true)
			{
				++connectedComponents;

				for (int i = 0; i < m_matrix[index].size(); ++i)
				{
					if (m_matrix[index][i])
					{
						nodes.push(i);
					}
				}
			}
		}

		// The graph is connected if the number of elements in the closed list equal the number of nodes in the graph
		return (connectedComponents == m_nodes.size());
	}

	void GUI::Update(IInput& input, double dt)
	{
		if (m_uiCurrentNode != -1)
		{
			for (const std::shared_ptr<IElement>& pIter : m_nodes[m_uiCurrentNode].elements)
			{
				pIter->Update(input, dt);
			}
		}
	}

	void GUI::Render(IRenderer& renderer)
	{
		if (m_uiCurrentNode != -1)
		{
			for (const std::shared_ptr<IElement>& pIter : m_nodes[m_uiCurrentNode].elements)
			{
				pIter->Render(renderer);
			}
		}
	}

}

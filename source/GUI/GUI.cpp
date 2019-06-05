#include "GUI.h"
#include <stack>
#include <set>
#include "Game.h"
#include "IMouseButtonListener.h"
#include "IMousePosListener.h"

namespace UI
{
	GUI::HANDLE GUI::CreateNode()
	{
		m_matrix.emplace_back();

		for (auto& iter : m_matrix)
		{
			iter.resize(m_matrix.size());
		}

		m_nodes.emplace_back();

		return m_matrix.size() - 1;
	}

	void GUI::SetNode(HANDLE id)
	{
		if (id < m_matrix.size())
		{
			if ((m_uiCurrentNode == std::numeric_limits<unsigned int>::max()) || m_matrix[m_uiCurrentNode][id])
			{
				m_uiCurrentNode = id;
			}
		}
	}

	GUI::HANDLE GUI::GetNode() const
	{
		return m_uiCurrentNode;
	}

	void GUI::AddElement(HANDLE id, const std::shared_ptr<IElement>& pElement)
	{
		if (id < m_matrix.size())
		{
			m_nodes[id].elements.push_back(pElement);
		}
	}

	void GUI::LinkNodes(HANDLE id, HANDLE id2, bool bBidirectional)
	{
		if (id < m_matrix.size() && id2 < m_matrix.size())
		{
			m_matrix[id][id2] = true;

			if (bBidirectional)
			{
				m_matrix[id2][id] = true;
			}
		}
	}

	bool GUI::IsConnected() const
	{
		if (m_uiCurrentNode == std::numeric_limits<unsigned int>::max())
			return false;

		// Use depth first search to find the connected components of the GUI graph.
		
		std::stack<HANDLE> nodes;
		std::set<HANDLE> closedList;
		nodes.push(m_uiCurrentNode);

		unsigned int connectedComponents = 0;
		while (!nodes.empty())
		{
			HANDLE index = nodes.top();
			nodes.pop();

			if (closedList.insert(index).second)
			{
				++connectedComponents;

				for (unsigned int i = 0; i < m_matrix[index].size(); ++i)
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

	void GUI::Init(IInput& input) {
		input.addMouseCursorPosCallback("gui", [this](glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration) -> bool {
			if (m_uiCurrentNode != std::numeric_limits<HANDLE>::max())
			{
				for (const std::shared_ptr<IElement>& pIter : m_nodes[m_uiCurrentNode].elements)
				{
					pIter->mousePosCallback(cursorPos, windowCursorPos, acceleration);
				}
			}
			return true;
		});

		input.addMouseButtonCallback("gui", [this](int key, bool pressed) -> bool {
			if (m_uiCurrentNode != std::numeric_limits<HANDLE>::max())
			{
				for (const std::shared_ptr<IElement>& pIter : m_nodes[m_uiCurrentNode].elements)
				{
					pIter->mouseButtonCallback(key, pressed);
				}
			}
			return true;
		});
	}

	void GUI::Render(IRenderer& renderer)
	{
		if (m_uiCurrentNode != std::numeric_limits<HANDLE>::max())
		{
			for (const std::shared_ptr<IElement>& pIter : m_nodes[m_uiCurrentNode].elements)
			{
				pIter->Render(renderer);
			}
		}
	}

}

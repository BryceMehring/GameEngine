#include "GUI.h"

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

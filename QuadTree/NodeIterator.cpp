#include "NodeIterator.h"
#include "QuadTree.h"

NodeIterator::NodeIterator(QuadTree* pNode) : m_pNode(pNode) {}
NodeIterator& NodeIterator::operator++()
{
	Increment();
	return *this;
}
NodeIterator& NodeIterator::operator++(int unused)
{
	return this->operator++();
}
QuadTree* NodeIterator::operator*()
{
	return m_pNode;
}
QuadTree* NodeIterator::operator->()
{
	return this->operator*();
}

NodeIterator& NodeIterator::operator=(QuadTree* pNode)
{
	m_pNode = pNode;
	return *this;
}

bool NodeIterator::operator==(QuadTree* pNode)
{
	return m_pNode == pNode;
}

bool NodeIterator::operator!=(QuadTree* pNode)
{
	return !this->operator==(pNode);
}

bool NodeIterator::operator==(const NodeIterator& node)
{
	return m_pNode == node.m_pNode;
}

bool NodeIterator::operator!=(const NodeIterator& node)
{
	return !this->operator==(node);
}

unsigned int NodeIterator::GetIndex() const
{
	return (m_pNode->m_Previous == nullptr) ? -1l : (4 - (&m_pNode->m_Previous->m_Nodes[3] - m_pNode));
}

void NodeIterator::LoopDown(unsigned int index)
{
	// Loop down
	m_pNode = &m_pNode->m_Nodes[index];
}
void NodeIterator::LoopUp()
{
	// Loop up
	unsigned int index = GetIndex();

	m_pNode = m_pNode->m_Previous;

	if(index >= 4)
	{
		do
		{
			index = GetIndex();
			m_pNode = m_pNode->m_Previous;

		} while(index == 4);
	}

	if(index != (unsigned int)-1)
	{
		LoopDown(index);
	}
}

void NodeIterator::Increment()
{
	if(m_pNode != nullptr)
	{
		if(m_pNode->IsDivided())
		{
			LoopDown(0);
		}
		else
		{
			LoopUp();
		}
	}
}

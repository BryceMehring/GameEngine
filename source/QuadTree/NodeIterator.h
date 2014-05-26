
#ifndef _NODEITERATOR_
#define _NODEITERATOR_

class NodeIterator
{
public:

	NodeIterator(class QuadTree* pNode = nullptr);

	NodeIterator& operator=(class QuadTree* pNode);
	NodeIterator& operator++();
	NodeIterator& operator++(int unused);

	bool operator ==(class QuadTree* pNode);
	bool operator !=(class QuadTree* pNode);

	bool operator ==(const NodeIterator&);
	bool operator !=(const NodeIterator&);

	class QuadTree* operator*();
	class QuadTree* operator->();

protected:

	class QuadTree* m_pNode;

	unsigned int GetIndex() const;
	void Increment();
	void LoopDown(unsigned int index);
	void LoopUp();
};

#endif // _NODEITERATOR_

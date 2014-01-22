#ifndef _PRIORITYQUEUE_
#define _PRIORITYQUEUE_

#include <vector>
#include <unordered_map>

#define NDEBUG

namespace cds
{
	// A self updating priority queue
	// Behavior is the same as std::priority_queue execpt for one small difference with Push()
	template< class T, class Container = std::vector<T>, class Compare = std::less<T> >
	class PriorityQueue
	{
	public:

		// Returns the element with the highest priority
		T& Top() { return m_data.front(); }
		const T& Top() const { return m_data.front(); }

		// Returns true if the queue is empty
		bool Empty() const { return m_data.empty(); }

		// Adds data if it is not already in the queue
		// If data is already in the queue, then it is moved to its correct location to satisfy
		// the heap property
		void Push(const T& data)
		{
			auto indexIter = m_index.find(data);
			if (indexIter != m_index.end())
			{
				Update(indexIter->second);
			}
			else
			{
				int i = m_data.size();

				m_data.push_back(data);
				m_index[data] = i;

				// Heapify
				PushUp(i);
			}

			assert(std::is_heap(m_data.begin(), m_data.end(), s_compareFunctor) == true);
		}

		// Removes the highest priority element
		void Pop()
		{
			if (!m_data.empty())
			{
				std::swap(m_data.front(), m_data.back());

				m_index[m_data.front()] = 0;

				m_index.erase(m_data.back());
				m_data.pop_back();
			}

			// Heapify
			PushDown(0);

			assert(std::is_heap(m_data.begin(), m_data.end(), s_compareFunctor) == true);
		}

	private:

		// The heap
		Container m_data;

		// Mappings from T to the index in the heap
		std::unordered_map<T, unsigned int> m_index;
		static const Compare s_compareFunctor;

		// Returns true if the element at index root can be pushed up in the heap
		// If so, then root is set to the parent to swap with
		bool CanPushUp(unsigned int& root) const
		{
			unsigned int parent = (root - 1) / 2;

			bool bCanPushUp = (root > 0) && (parent < m_data.size()) && (s_compareFunctor(m_data.at(parent), m_data[root]));

			if (bCanPushUp)
			{
				root = parent;
			}

			return bCanPushUp;
		}

		// Pushes the data at root up in the heap to satisfy the heap property
		// Returns the new index of the root
		int PushUp(unsigned int root)
		{
			bool bPush = true;
			while (bPush)
			{
				unsigned int parent = root;
				if ((bPush = CanPushUp(parent)))
				{
					Swap(root, parent);
				}
			}

			return root;
		}

		// Returns true if root can be pushed down
		// If so, root is set to the new index to swap with in the heap
		bool CanPushDown(unsigned int& root) const
		{
			unsigned int left = root * 2 + 1;
			unsigned int right = root * 2 + 2;
			unsigned int newRoot = root;

			if (left < m_data.size() && s_compareFunctor(m_data[root], m_data[left]))
			{
				newRoot = left;
			}
			if (right < m_data.size() && s_compareFunctor(m_data[newRoot], m_data[right]))
			{
				newRoot = right;
			}

			bool bCanPushDown = root < newRoot;
			if (bCanPushDown)
			{
				root = newRoot;
			}

			return bCanPushDown;
		}

		// Pushes root down in the heap to satisfy the heap property 
		void PushDown(unsigned int root)
		{
			bool bLoop = true;

			while (bLoop)
			{
				unsigned int newRoot = root;
				if ((bLoop = CanPushDown(newRoot)))
				{
					Swap(root, newRoot);
				}
			}
		}

		// Swap nodes a and b within the heap
		// a = old root, which is set to b after to the swap
		// b = new root
		void Swap(unsigned int& a, unsigned int b)
		{
			std::swap(m_data[a], m_data[b]);
			m_index[m_data[b]] = b;
			m_index[m_data[a]] = a;

			a = b;
		}

		// Tries to push the root up, then down to the new correct location in the heap
		void Update(unsigned int root)
		{
			PushDown(PushUp(root));
		}
	};

	template< class T, class Container, class Compare >
	const Compare PriorityQueue<T, Container, Compare>::s_compareFunctor;
}

#endif // _PRIORITYQUEUE_
#ifndef _GUI_
#define _GUI_

#include "IInput.h"
#include "IRenderer.h"
#include "IElement.h"

#include <memory>

namespace UI
{

	// Defines a simple graph based GUI
	class GUI
	{
	public:

		using HANDLE = unsigned int;

		// Returns the new node added to the graph
		HANDLE CreateNode();

		// Sets the current node for which updating and rendering take place
		void SetNode(HANDLE id);

		// Returns the current node in the graph for which updating and rendering take place
		HANDLE GetNode() const;

		// Adds pElement to the node in the graph specified by id
		void AddElement(HANDLE id, const std::shared_ptr<IElement>& pElement);

		// Links nodes id and id2 bidirectionally 
		void LinkNodes(HANDLE id, HANDLE id2);

		// Returns true if the graph has links to all of the nodes in the graph,
		// meaning that one can reach every node in the graph
		bool IsConnected() const;

		// Updates all of the elements in the current node of the graph
		void Update(IInput& input, double dt);

		// Renders all of the elements in the current node of the graph
		void Render(IRenderer& renderer);

	private:

		struct Node
		{
			// A list of all gui elements in the node
			std::vector<std::shared_ptr<IElement>> elements;
		};

		std::vector<std::vector<bool>> m_matrix;
		std::vector<Node> m_nodes;

		HANDLE m_uiCurrentNode = std::numeric_limits<HANDLE>::max();

	};

}

#endif // _GUI_

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

		// Returns the new node added to the graph
		unsigned int CreateNode();

		// Sets the current node for which updating and rendering take place
		void SetNode(unsigned int id);

		// Returns the current node in the graph for which updating and rendering take place
		unsigned int GetNode() const;

		// Adds pElement to the node in the graph specified by id
		void AddElement(unsigned int id, const std::shared_ptr<IElement>& pElement);

		// Links nodes id and id2 bidirectionally 
		void LinkNodes(unsigned int id, unsigned int id2);

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

		unsigned int m_uiCurrentNode = -1;

	};

}

#endif // _GUI_
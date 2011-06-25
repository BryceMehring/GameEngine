#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "UI.h"

class GraphEdge;

class GraphVertex
{
public:

	GraphVertex() {}
	virtual ~GraphVertex() {}

	void AddEdge(GraphEdge* pNeighbor) { m_Neighbors.push_back(pNeighbor); }
	std::vector<GraphEdge*>& GetEdges() { return m_Neighbors; }

	// Cost used during path searching
	void SetPathSearchCost(const float fCost) { m_fPathSearchCost = fCost; }
	float GetPathSearchCost() const { return  m_fPathSearchCost; } 

	//Predecessor used during the search
	void SetPredecessor(GraphVertex* pPred) { m_pPredecessor = pPred; }
	GraphVertex* GetPredecessor() const { return m_pPredecessor; }

protected:
	std::vector<GraphEdge*> m_Neighbors;

	float m_fPathSearchCost;

	GraphVertex* m_pPredecessor;
};

class GraphEdge
{
public:	
	GraphEdge() {}
	virtual ~GraphEdge() {}

	GraphVertex* GetTail() const		{ return m_pTail;  }
	GraphVertex* GetHead() const		{ return m_pHead;  }
	void SetTail(GraphVertex* pVert)	{ m_pTail = pVert; }
	void SetHead(GraphVertex* pVert)	{ m_pHead = pVert; }
protected:
	GraphVertex* m_pTail;
	GraphVertex* m_pHead;
};

class PosNode : public GraphVertex
{
public:
	PosNode(const D3DXVECTOR3& pos) : m_Pos(pos) {}

	//Get
	const D3DXVECTOR3& GetPos() const { return m_Pos; }

	//Set
	void SetPos(const D3DXVECTOR3& pos) { m_Pos = pos; }
protected:
	D3DXVECTOR3 m_Pos;
};


class GraphVertexSearchCostComparer
{
public:
	bool operator()(const GraphVertex* v1, const GraphVertex* v2) const
	{
		return (v1->GetPathSearchCost() > v2->GetPathSearchCost());
	}
};


template< class VERTEX, class EDGE >
class Graph
{
public:
	Graph()
	{
		m_Verts.reserve(50);
		m_Edges.reserve(50);
	}
	virtual ~Graph()
	{
		for(vector<VERTEX*>::iterator VertIter = m_Verts.begin(); VertIter != m_Verts.end(); ++VertIter) { delete (*VertIter); }
		for(vector<EDGE*>::iterator EdgeIter = m_Edges.begin(); EdgeIter != m_Edges.end(); ++EdgeIter) { delete (*EdgeIter); }

		m_Verts.clear();
		m_Edges.clear();
	}

	// use these to get the vertex in the ui
	VERTEX* GetVertex(const int iIndex) { return m_Verts[iIndex]; }
	EDGE* GetEdge(const int iIndex) { return m_Edges[iIndex]; }

	void AddVertex(VERTEX* pVert) { m_Verts.push_back(pVert); }
	void CreateEdge(VERTEX* pTail, VERTEX* pHead, bool bTwoWay = true)
	{
		// Create an edge from the first vertex to the second
		EDGE* pEdge = new EDGE;
		pEdge->SetTail(pTail);
		pEdge->SetHead(pHead);
		pTail->AddEdge(pEdge);

		// If the edge is marked as two way, then create a new edge going the other direction
		if(bTwoWay)
		{
			EDGE* pEdge2 = new EDGE;
			pEdge2->SetTail(pHead);
			pEdge2->SetHead(pTail);
			pHead->AddEdge(pEdge2);
		}
	}

	template< class T >
	float ComputeBestPath(VERTEX* pStart, VERTEX* pGoal, const T& Functor , vector<VERTEX*>& path)
	{
		InitializePathSearch();
		priority_queue<VERTEX*,vector<VERTEX*>,GraphVertexSearchCostComparer> fringe;

		pStart->SetPathSearchCost(0.f);

		VERTEX* pCurrentVertex = pStart;

		fringe.push(pStart);
		float fTotalCostToGoal = FLT_MAX;
		while(!fringe.empty())
		{
			vector<EDGE*>& edges = pCurrentVertex->GetEdges();

			for(vector<EDGE*>::iterator EdgeIter = edges.begin(); EdgeIter != edges.end(); ++EdgeIter)
			{
				EDGE* pEdge = *EdgeIter;
				VERTEX* pNeighbor = static_cast<VERTEX*>(pEdge->GetHead());
				
				//Don't consider the vertex we just came from
				if(pNeighbor == pCurrentVertex->GetPredecessor()) continue;

				float fEdgeCost = Functor(pEdge);
				float fNewCostToThisVertex = (pCurrentVertex->GetPathSearchCost() + fEdgeCost);
				float fCurrentCostToThisVertex = pNeighbor->GetPathSearchCost();

				if((fNewCostToThisVertex < fCurrentCostToThisVertex) && (fNewCostToThisVertex < fTotalCostToGoal))
				{
					pNeighbor->SetPathSearchCost(fNewCostToThisVertex);
					pNeighbor->SetPredecessor(pCurrentVertex);

					if(pNeighbor == pGoal)
					{
						fTotalCostToGoal = fNewCostToThisVertex;
					}

					fringe.push(pNeighbor);
				}
			}

			fringe.pop();
			if(!fringe.empty())
			{
				pCurrentVertex = fringe.top();
			}
		}

		// Construct path if found
		if(fTotalCostToGoal < FLT_MAX)
		{
			VERTEX* pPathVertex = pGoal;
			do
			{
				path.push_back(pPathVertex);
				pPathVertex = static_cast<VERTEX*>(pPathVertex->GetPredecessor());
			}
			while(pPathVertex != pStart);
			path.push_back(pStart);
		}

		return fTotalCostToGoal;
	}



protected:
	vector<VERTEX*> m_Verts;
	vector<EDGE*> m_Edges;

	void InitializePathSearch()
	{
		for(vector<VERTEX*>::iterator iter = m_Verts.begin(); iter != m_Verts.end(); ++iter)
		{
			(*iter)->SetPathSearchCost(FLT_MAX);
			(*iter)->SetPredecessor(NULL);
		}
	}
};

class NaviGraph : public Graph<NavigationNode,GraphEdge>
{
public:

	class NaviEdgeCost
	{
	public:

		float operator()(const GraphEdge* const pEdge) const
		{
			const NavigationNode* const pTail = static_cast<NavigationNode*>(pEdge->GetTail());
			const NavigationNode* const pHead = static_cast<NavigationNode*>(pEdge->GetHead());

			return Algorithm::FindDistance(pTail->GetPos(),pHead->GetPos());
		}

	};

	NaviGraph() {}
	virtual ~NaviGraph() {}

	NavigationNode* FindVertex(const int Id)
	{
		for(vector<NavigationNode*>::iterator NodeIter = m_Verts.begin(); NodeIter != m_Verts.end(); ++NodeIter)
		{
			NavigationNode* pNode = *NodeIter; 

			if(dbSpriteCollision(Id,pNode->GetNodeId()))
			{
				return pNode;
			}
		}

		return NULL;
	}

};

// todo: need to implement
// http://sourcemaking.com/design_patterns/state/cpp/1
class UIGraph //: public Graph<UIVertex,UIEdge>
{

};


#endif
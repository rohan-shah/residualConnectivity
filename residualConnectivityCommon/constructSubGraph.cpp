#include "constructSubGraph.h"
namespace residualConnectivity
{
	void constructSubGraph(subGraphType& graph, std::vector<int>& graphVertices, const context& contextObj, const vertexState* stateRef)
	{
		const context::inputGraph& originalGraph = contextObj.getGraph();
		graphVertices.clear();
		std::size_t nVertices = boost::num_vertices(originalGraph);

		//graphVertices[i] is the vertex in the global graph that corresponds to vertex i in the returned graph
		//inverse is the inverse mapping, with 0's everywhere else. 
		int* inverse = (int*)alloca(nVertices * sizeof(int));
		//0 is used to identify a point which is not going to be contained within the returned graph
		memset(inverse, 0, sizeof(int)*nVertices);

		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(stateRef[i].state & (UNFIXED_MASK | ON_MASK))
			{
				graphVertices.push_back((int)i);
				inverse[i] = (int)graphVertices.size();
			}
		}
		graph.clear();
		//graph = subGraphType(graphVertices.size());
		for(std::size_t i = 0; i < graphVertices.size(); i++)
		{
			context::inputGraph::out_edge_iterator start, end;
			boost::tie(start, end) = boost::out_edges(graphVertices[i], originalGraph);
			while(start != end)
			{
				if(inverse[start->m_target] > 0 && (int)i < inverse[start->m_target] - 1)
				{
					boost::add_edge((int)i, inverse[start->m_target] - 1, graph);
				}
				start++;
			}
		}
	}
}

#ifndef FIND_FIXED_ON_VISITOR_HEADER_GUARD
#define FIND_FIXED_ON_VISITOR_HEADER_GUARD
#include <boost/graph/connected_components.hpp>
#include "vertexState.h"
#include <vector>
namespace residualConnectivity
{
	struct findFixedOnVisitor : public boost::default_dfs_visitor
	{
	public:
		findFixedOnVisitor(const vertexState* state, std::vector<int>& graphVertices)
			:found(false), state(state), graphVertices(graphVertices)
		{}
		template<typename Vertex, typename Graph> void discover_vertex(Vertex u, const Graph& g)
		{
			found |= (state[graphVertices[u]].state == FIXED_ON);
		}
		bool found;
		const vertexState* state;
		std::vector<int>& graphVertices;
	};
}
#endif

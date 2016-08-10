#ifndef RESIDUAL_CONNECTIVITY_CONSTRUCT_SUBGRAPH_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_CONSTRUCT_SUBGRAPH_HEADER_GUARD
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include "context.h"
namespace residualConnectivity
{
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > subGraphType;
	void constructSubGraph(subGraphType& graph, std::vector<int>& graphVertices, const context& contextObj, const vertexState* stateRef);
}
#endif

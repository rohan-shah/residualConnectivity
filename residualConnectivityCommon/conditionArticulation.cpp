#include "conditionArticulation.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/biconnected_components.hpp>
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
	void conditionArticulation(boost::shared_array<vertexState> state, mpfr_class& weight, const context& contextObj, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& graph)
	{
		//construct subgraph
		graph.clear();
		//Reuse components vector here
		std::vector<int>& graphVertices = scratch;
		{
			constructSubGraph(graph, graphVertices, contextObj, state.get());
			//assign edge indices
			subGraphType::edge_iterator start, end;
			int edgeIndex = 0;
			boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
			boost::tie(start, end) = boost::edges(graph);
			for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
		}
		//get out biconnected components of helper graph (which has different vertex ids, remember)
		std::vector<std::size_t> articulationVertices;
		boost::articulation_points(graph, std::back_inserter(articulationVertices));
	
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(boost::num_vertices(graph), Color::white());
		findFixedOnVisitor fixedVisitor(state.get(), graphVertices);
		const std::vector<mpfr_class> operationalProbabilities = contextObj.getOperationalProbabilities();

		for(std::vector<std::size_t>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
		{
			if(state[graphVertices[*i]].state != FIXED_ON)
			{
				std::fill(colorMap.begin(), colorMap.end(), Color::white());
				colorMap[*i] = Color::black();

				subGraphType::out_edge_iterator current, end;
				boost::tie(current, end) = boost::out_edges(*i, graph);
				int nComponentsWithFixedOnVertices = 0;
				for(; current != end; current++)
				{
					std::size_t otherVertex = current->m_target;
					if(colorMap[otherVertex] != Color::black())
					{
						fixedVisitor.found = false;
						boost::detail::depth_first_visit_restricted_impl(graph, otherVertex, fixedVisitor, &(colorMap[0]), subGraphStack, boost::detail::nontruth2());
						if(fixedVisitor.found) nComponentsWithFixedOnVertices++;
						if(nComponentsWithFixedOnVertices > 1) break;
					}
				}
				if(nComponentsWithFixedOnVertices > 1)
				{
					state[graphVertices[*i]].state = FIXED_ON;
					weight *= operationalProbabilities[graphVertices[*i]];
				}
			}
		}
	}
}

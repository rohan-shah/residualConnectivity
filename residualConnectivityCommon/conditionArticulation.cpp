#include "conditionArticulation.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "findFixedOnVisitor.h"
namespace residualConnectivity
{
	void conditionArticulation(boost::shared_array<vertexState> state, mpfr_class& weight, const context& contextObj, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
	{
		const context::inputGraph& graph = contextObj.getGraph();
		filteredGraphType filtered(graph, boost::keep_all(), filterByStateMask(state.get(), UNFIXED_MASK | ON_MASK));
		//get out biconnected components of helper graph (which has different vertex ids, remember)
		std::vector<std::size_t> articulationVertices;
		boost::articulation_points(filtered, std::back_inserter(articulationVertices));
	
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(boost::num_vertices(contextObj.getGraph()), Color::white());
		findFixedOnVisitor fixedVisitor(state.get());
		const std::vector<mpfr_class> operationalProbabilities = contextObj.getOperationalProbabilities();

		for(std::vector<std::size_t>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
		{
			if(state[*i].state != FIXED_ON)
			{
				std::fill(colorMap.begin(), colorMap.end(), Color::white());
				colorMap[*i] = Color::black();

				filteredGraphType::out_edge_iterator current, end;
				boost::tie(current, end) = boost::out_edges(*i, filtered);
				int nComponentsWithFixedOnVertices = 0;
				for(; current != end; current++)
				{
					std::size_t otherVertex = current->m_target;
					if(colorMap[otherVertex] != Color::black())
					{
						fixedVisitor.found = false;
						boost::detail::depth_first_visit_restricted_impl(filtered, otherVertex, fixedVisitor, &(colorMap[0]), filteredGraphStack, boost::detail::nontruth2());
						if(fixedVisitor.found) nComponentsWithFixedOnVertices++;
						if(nComponentsWithFixedOnVertices > 1) break;
					}
				}
				if(nComponentsWithFixedOnVertices > 1)
				{
					state[*i].state = FIXED_ON;
					weight *= operationalProbabilities[*i];
				}
			}
		}
	}
}

#ifndef IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#define IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/shared_ptr.hpp>
#include "Context.h"
#include "depth_first_search_restricted.hpp"
namespace discreteGermGrain
{
	namespace singleComponent
	{
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > graphType;
	}
	bool isSingleComponentPossible(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
	bool isSingleComponentAllOn(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
	bool isSingleComponentSpecified(Context const& context, const std::vector<singleComponent::graphType::vertex_descriptor>& specifiedVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
}
#endif

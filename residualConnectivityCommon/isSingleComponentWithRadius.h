#ifndef IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#define IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/shared_ptr.hpp>
#include "Context.h"
#include "depth_first_search_restricted.hpp"
namespace residualConnectivity
{
	bool isSingleComponentPossible(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
	bool isSingleComponentAllOn(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
	bool isSingleComponentSpecified(Context const& context, const std::vector<Context::inputGraph::vertex_descriptor>& specifiedVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
	bool partIsSingleComponent(Context const& context, const vertexState* state, const std::vector<Context::inputGraph::vertex_descriptor>& importantPoints, std::vector<Context::inputGraph::vertex_descriptor>& otherVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
}
#endif

#ifndef IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#define IS_SINGLE_COMPONENT_WITH_RADIUS_HEADER_GUARD
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/shared_ptr.hpp>
#include "context.h"
#include "depth_first_search_restricted.hpp"
namespace residualConnectivity
{
	bool isSingleComponentPossible(context const& contextObj, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
	bool isSingleComponentAllOn(context const& contextObj, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
	bool isSingleComponentSpecified(context const& contextObj, const std::vector<context::inputGraph::vertex_descriptor>& specifiedVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
	bool partIsSingleComponent(context const& contextObj, const vertexState* state, const std::vector<context::inputGraph::vertex_descriptor>& importantPoints, std::vector<context::inputGraph::vertex_descriptor>& otherVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
}
#endif

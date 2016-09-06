#ifndef RESIDUAL_CONNECTIVITY_CONDITION_ARTICULATION_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_CONDITION_ARTICULATION_HEADER_GUARD
#include "context.h"
#include <vector>
#include "vertexState.h"
#include "depth_first_search_restricted.hpp"
#include <boost/graph/filtered_graph.hpp>
#include "filterByStateMask.h"
namespace residualConnectivity
{
	typedef boost::filtered_graph<context::inputGraph, boost::keep_all, filterByStateMask> filteredGraphType;
	void conditionArticulation(boost::shared_array<vertexState> state, mpfr_class& weight, const context& contextObj, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack);
}
#endif

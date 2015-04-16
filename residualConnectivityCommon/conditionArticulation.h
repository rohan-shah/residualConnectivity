#ifndef DISCRETE_GERM_GRAIN_CONDITION_ARTICULATION_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_CONDITION_ARTICULATION_HEADER_GUARD
#include "Context.h"
#include <vector>
#include "vertexState.h"
#include "depth_first_search_restricted.hpp"
#include "constructSubGraph.h"
namespace discreteGermGrain
{
	void conditionArticulation(boost::shared_array<vertexState> state, mpfr_class& weight, const Context& context, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack);
}
#endif

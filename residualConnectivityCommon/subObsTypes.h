#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_TYPES_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_TYPES_HEADER_GUARD
#include <vector>
#include "depth_first_search_restricted.hpp"
#include "Context.h"
#include "constructSubGraph.h"
namespace discreteGermGrain
{
	namespace obs
	{
		struct basicConstructorType
		{
		};
		struct withWeightConstructorType
		{
		public:
			withWeightConstructorType();
			mpfr_class weight;
		};
	}
	namespace subObs
	{
		struct basicConstructorType
		{
		public:
			basicConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack;
		};
		struct withWeightConstructorType
		{
		public:
			withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack;
			boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack;
			mpfr_class weight;
		};
	}
}
#endif

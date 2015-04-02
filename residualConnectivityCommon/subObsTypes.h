#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_TYPES_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_TYPES_HEADER_GUARD
#include <vector>
#include "depth_first_search_restricted.hpp"
#include "Context.h"
namespace discreteGermGrain
{
	namespace obs
	{
		struct usingBiconnectedComponentsConstructorType
		{
		};
		struct basicConstructorType
		{
		};
		struct usingCutVerticesConstructorType
		{
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
		struct usingBiconnectedComponentsConstructorType
		{
		public:
			usingBiconnectedComponentsConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack;
		};
		struct usingCutVerticesConstructorType
		{
		public:
			usingCutVerticesConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack;
		};
	}
}
#endif

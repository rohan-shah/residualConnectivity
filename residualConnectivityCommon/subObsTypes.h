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
		public:
			usingBiconnectedComponentsConstructorType();
			mpfr_class weight;
		};
		struct basicConstructorType
		{
		};
		struct usingCutVerticesConstructorType
		{
		public:
			usingCutVerticesConstructorType();
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
		struct usingBiconnectedComponentsConstructorType
		{
		public:
			usingBiconnectedComponentsConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack;
			mpfr_class weight;
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

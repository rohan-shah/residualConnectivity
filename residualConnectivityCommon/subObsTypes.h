#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_TYPES_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_TYPES_HEADER_GUARD
#include <vector>
#include "depth_first_search_restricted.hpp"
#include "context.h"
#include "constructSubGraph.h"
namespace residualConnectivity
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
		struct weightAndCountConstructorType
		{
			weightAndCountConstructorType()
			{}
			int count;
			mpfr_class weight;
		};
	}
	namespace subObs
	{
		struct basicConstructorType
		{
		public:
			basicConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack;
		};
		struct withWeightConstructorType
		{
		public:
			withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph);
			std::vector<int>& components;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack;
			boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack;
			mpfr_class weight;
			subGraphType& subGraph;
		};
		struct articulationConditioningForResamplingConstructorType : public withWeightConstructorType
		{
			articulationConditioningForResamplingConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph);
			bool useConditioning;
		};
		struct articulationConditioningSameCountConstructorType : public articulationConditioningForResamplingConstructorType
		{
			articulationConditioningSameCountConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph);
			int count;
		};
	}
}
#endif

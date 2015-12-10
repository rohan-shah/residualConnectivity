#include "subObsTypes.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningForResamplingConstructorType::articulationConditioningForResamplingConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph)
			:withWeightConstructorType(components, stack, subGraphStack, subGraph)
		{}
		withWeightConstructorType::withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph)
			:components(components), stack(stack), subGraphStack(subGraphStack), subGraph(subGraph)
		{}
		basicConstructorType::basicConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
			:components(components), stack(stack)
		{}
	}
	namespace obs
	{
		withWeightConstructorType::withWeightConstructorType()
		{}
	}
}

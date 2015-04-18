#include "subObsTypes.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		usingMultipleLevelsForResamplingConstructorType::usingMultipleLevelsForResamplingConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack)
			:withWeightConstructorType(components, stack, subGraphStack)
		{}
		withWeightConstructorType::withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack)
			:components(components), stack(stack), subGraphStack(subGraphStack)
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

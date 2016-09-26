#include "subObsTypes.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningSameCountConstructorType::articulationConditioningSameCountConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
			: withWeightConstructorType(components, stack, filteredGraphStack)
		{}
		withWeightConstructorType::withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
			:components(components), stack(stack), filteredGraphStack(filteredGraphStack)
		{}
		basicConstructorType::basicConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
			:components(components), stack(stack)
		{}
		withWeightImportanceConstructorType::withWeightImportanceConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
			: withWeightConstructorType(components, stack, filteredGraphStack), importanceProbabilities(NULL)
		{}
	}
	namespace obs
	{
		withWeightConstructorType::withWeightConstructorType()
		{}
	}
}

#include "subObsTypes.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		withWeightConstructorType::withWeightConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
			:components(components), stack(stack)
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

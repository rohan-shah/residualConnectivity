#include "subObsTypes.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		usingBiconnectedComponentsConstructorType::usingBiconnectedComponentsConstructorType(std::vector<int>& components, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
			:components(components), stack(stack)
		{}
	}
}

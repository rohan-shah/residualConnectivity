#include "obs/basic.h"
namespace residualConnectivity
{
	namespace obs
	{
		basic::basic(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&)
			: ::residualConnectivity::withSub(context, state)
		{}
		basic::basic(Context const& context, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(context, randomSource)
		{}
		void basic::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
		}
		basic::basic(basic&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other))
		{}
	}
}

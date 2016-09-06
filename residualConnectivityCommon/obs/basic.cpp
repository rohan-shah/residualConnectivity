#include "obs/basic.h"
namespace residualConnectivity
{
	namespace obs
	{
		basic::basic(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&)
			: ::residualConnectivity::withSub(contextObj, state)
		{}
		basic::basic(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource)
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

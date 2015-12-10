#include "obs/usingBiconnectedComponents.h"
namespace residualConnectivity
{
	namespace obs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(context, state), weight(otherData.weight)
		{}
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(context, randomSource), weight(1)
		{}
		void usingBiconnectedComponents::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		usingBiconnectedComponents::usingBiconnectedComponents(usingBiconnectedComponents&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& usingBiconnectedComponents::getWeight() const
		{
			return weight;
		}
	}
}

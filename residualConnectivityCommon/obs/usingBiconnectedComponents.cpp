#include "obs/usingBiconnectedComponents.h"
namespace residualConnectivity
{
	namespace obs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight)
		{}
		usingBiconnectedComponents::usingBiconnectedComponents(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource), weight(1)
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

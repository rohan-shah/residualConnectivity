#include "obs/articulationConditioning.h"
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioning::articulationConditioning(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight)
		{}
		articulationConditioning::articulationConditioning(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource), weight(1)
		{}
		void articulationConditioning::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		articulationConditioning::articulationConditioning(articulationConditioning&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& articulationConditioning::getWeight() const
		{
			return weight;
		}
	}
}

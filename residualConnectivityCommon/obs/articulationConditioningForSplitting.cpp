#include "obs/articulationConditioningForSplitting.h"
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioningForSplitting::articulationConditioningForSplitting(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight)
		{}
		articulationConditioningForSplitting::articulationConditioningForSplitting(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource), weight(1)
		{}
		void articulationConditioningForSplitting::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		articulationConditioningForSplitting::articulationConditioningForSplitting(articulationConditioningForSplitting&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& articulationConditioningForSplitting::getWeight() const
		{
			return weight;
		}
	}
}

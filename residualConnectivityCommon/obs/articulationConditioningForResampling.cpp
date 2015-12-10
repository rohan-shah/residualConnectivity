#include "obs/articulationConditioningForResampling.h"
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioningForResampling::articulationConditioningForResampling(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(context, state), weight(otherData.weight)
		{}
		articulationConditioningForResampling::articulationConditioningForResampling(Context const& context, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(context, randomSource), weight(1)
		{}
		void articulationConditioningForResampling::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		articulationConditioningForResampling::articulationConditioningForResampling(articulationConditioningForResampling&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& articulationConditioningForResampling::getWeight() const
		{
			return weight;
		}
	}
}

#include "obs/articulationConditioningForResampling.h"
namespace discreteGermGrain
{
	namespace obs
	{
		articulationConditioningForResampling::articulationConditioningForResampling(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::withSub(context, state), weight(otherData.weight)
		{}
		articulationConditioningForResampling::articulationConditioningForResampling(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource), weight(1)
		{}
		void articulationConditioningForResampling::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		articulationConditioningForResampling::articulationConditioningForResampling(articulationConditioningForResampling&& other)
			: ::discreteGermGrain::withSub(static_cast<::discreteGermGrain::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& articulationConditioningForResampling::getWeight() const
		{
			return weight;
		}
	}
}

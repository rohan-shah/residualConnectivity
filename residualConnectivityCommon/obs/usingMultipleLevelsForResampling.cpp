#include "obs/usingMultipleLevelsForResampling.h"
namespace discreteGermGrain
{
	namespace obs
	{
		usingMultipleLevelsForResampling::usingMultipleLevelsForResampling(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::withSub(context, state), weight(otherData.weight)
		{}
		usingMultipleLevelsForResampling::usingMultipleLevelsForResampling(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource), weight(1)
		{}
		void usingMultipleLevelsForResampling::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		usingMultipleLevelsForResampling::usingMultipleLevelsForResampling(usingMultipleLevelsForResampling&& other)
			: ::discreteGermGrain::withSub(static_cast<::discreteGermGrain::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& usingMultipleLevelsForResampling::getWeight() const
		{
			return weight;
		}
	}
}

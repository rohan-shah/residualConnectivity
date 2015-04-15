#include "obs/usingMultipleLevelsConditioning.h"
namespace discreteGermGrain
{
	namespace obs
	{
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::withSub(context, state), weight(otherData.weight)
		{}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource), weight(1)
		{}
		void usingMultipleLevelsConditioning::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(usingMultipleLevelsConditioning&& other)
			: ::discreteGermGrain::withSub(static_cast<::discreteGermGrain::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& usingMultipleLevelsConditioning::getWeight() const
		{
			return weight;
		}
	}
}

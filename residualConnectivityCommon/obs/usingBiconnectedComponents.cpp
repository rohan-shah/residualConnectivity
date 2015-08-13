#include "obs/usingBiconnectedComponents.h"
namespace discreteGermGrain
{
	namespace obs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::withSub(context, state), weight(otherData.weight)
		{}
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource), weight(1)
		{}
		void usingBiconnectedComponents::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
			other.weight = weight;
		}
		usingBiconnectedComponents::usingBiconnectedComponents(usingBiconnectedComponents&& other)
			: ::discreteGermGrain::withSub(static_cast< ::discreteGermGrain::withSub&&>(other)), weight(other.weight)
		{}
		const mpfr_class& usingBiconnectedComponents::getWeight() const
		{
			return weight;
		}
	}
}

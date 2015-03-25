#include "obs/usingBiconnectedComponents.h"
namespace discreteGermGrain
{
	namespace obs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::usingBiconnectedComponentsConstructorType&)
			: ::discreteGermGrain::withSub(context, state)
		{}
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource)
		{}
		void usingBiconnectedComponents::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
		}
	}
}

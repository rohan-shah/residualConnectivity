#include "obs/basic.h"
namespace discreteGermGrain
{
	namespace obs
	{
		basic::basic(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&)
			: ::discreteGermGrain::withSub(context, state)
		{}
		basic::basic(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource)
		{}
		void basic::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
		}
		basic::basic(basic&& other)
			: ::discreteGermGrain::withSub(static_cast< ::discreteGermGrain::withSub&&>(other))
		{}
	}
}

#include "obs/usingCutVertices.h"
namespace discreteGermGrain
{
	namespace obs
	{
		usingCutVertices::usingCutVertices(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::usingCutVerticesConstructorType&)
			: ::discreteGermGrain::withSub(context, state)
		{}
		usingCutVertices::usingCutVertices(Context const& context, boost::mt19937& randomSource)
			: ::discreteGermGrain::withSub(context, randomSource)
		{}
		void usingCutVertices::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::discreteGermGrain::withSub::getSubObservation(radius, newState);
		}
	}
}

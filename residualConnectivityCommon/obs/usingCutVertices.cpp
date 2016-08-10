#include "obs/usingCutVertices.h"
namespace residualConnectivity
{
	namespace obs
	{
		usingCutVertices::usingCutVertices(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight)
		{}
		usingCutVertices::usingCutVertices(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource)
		{}
		void usingCutVertices::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
		}
	}
}

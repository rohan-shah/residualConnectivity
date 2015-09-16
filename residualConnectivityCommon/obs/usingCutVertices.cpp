#include "obs/usingCutVertices.h"
namespace residualConnectivity
{
	namespace obs
	{
		usingCutVertices::usingCutVertices(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType& otherData)
			: ::residualConnectivity::withSub(context, state), weight(otherData.weight)
		{}
		usingCutVertices::usingCutVertices(Context const& context, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(context, randomSource)
		{}
		void usingCutVertices::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
		}
	}
}

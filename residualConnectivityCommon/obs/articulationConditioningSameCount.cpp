#include "obs/articulationConditioningSameCount.h"
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioningSameCount::articulationConditioningSameCount(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::weightAndCountConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight), nUpVertices(otherData.count)
		{}
		articulationConditioningSameCount::articulationConditioningSameCount(context const& contextObj, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj, randomSource), weight(1)
		{
			nUpVertices = 0;
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(state[i].state & ON_MASK) nUpVertices++;
			}
		}
		void articulationConditioningSameCount::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			other.weight = weight;
			other.count = nUpVertices;
		}
		articulationConditioningSameCount::articulationConditioningSameCount(articulationConditioningSameCount&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight), nUpVertices(other.nUpVertices)
		{}
		const mpfr_class& articulationConditioningSameCount::getWeight() const
		{
			return weight;
		}
	}
}

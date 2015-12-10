#include "subObs/basic.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
namespace residualConnectivity
{
	namespace subObs
	{
		basic::basic(Context const& context, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::basicConstructorType& otherData)
			: ::residualConnectivity::subObs::subObsWithRadius(context, state, radius)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
		}
		bool basic::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		basic::basic(basic&& other)
			: ::residualConnectivity::subObs::subObsWithRadius(static_cast< ::residualConnectivity::subObs::subObsWithRadius&&>(other))
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void basic::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType&)const
		{
			boost::random::bernoulli_distribution<double> vertexDistribution(context.getOperationalProbabilityD());
			std::size_t nVertices = context.nVertices();
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					if(vertexDistribution(randomSource))
					{
						outputState[i].state = UNFIXED_ON;
					}
					else outputState[i].state = UNFIXED_OFF;
				}
			}
		}
	}
}

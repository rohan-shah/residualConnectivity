#include "subObs/subObs.h"
#include <boost/random/bernoulli_distribution.hpp>
namespace residualConnectivity
{
	namespace subObs
	{
		subObsWithRadius::subObsWithRadius(Context const& context, boost::shared_array<const vertexState> state, int radius)
			: ::residualConnectivity::subObs::subObs(context, state), radius(radius)
		{}
		subObsWithRadius::subObsWithRadius(subObsWithRadius&& other)
			: ::residualConnectivity::subObs::subObs(static_cast< ::residualConnectivity::subObs::subObs&&>(other)), radius(other.radius)
		{}
		subObs::subObs(Context const& context, boost::shared_array<const vertexState> state)
			: ::residualConnectivity::observation(context, state)
		{}
		subObs::subObs(subObs&& other)
			: ::residualConnectivity::observation(static_cast< ::residualConnectivity::observation&&>(other))
		{}
		subObs::subObs(const subObs& other)
			: ::residualConnectivity::observation(static_cast<const ::residualConnectivity::observation&>(other))
		{}
		subObsWithRadius::subObsWithRadius(const subObsWithRadius& other)
			: ::residualConnectivity::subObs::subObs(static_cast<const ::residualConnectivity::subObs::subObs&>(other)), radius(other.radius)
		{}
		void subObs::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType&) const
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

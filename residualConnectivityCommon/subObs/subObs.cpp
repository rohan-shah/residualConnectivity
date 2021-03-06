#include "subObs/subObs.h"
#include <boost/random/bernoulli_distribution.hpp>
namespace residualConnectivity
{
	namespace subObs
	{
		subObsWithRadius& subObsWithRadius::operator=(const subObsWithRadius& other)
		{
			subObs::operator=(other);
			radius = other.radius;
			return *this;
		}
		subObsWithRadius::subObsWithRadius(context const& contextObj, boost::shared_array<const vertexState> state, int radius)
			: ::residualConnectivity::subObs::subObs(contextObj, state), radius(radius)
		{}
		subObsWithRadius::subObsWithRadius(subObsWithRadius&& other)
			: ::residualConnectivity::subObs::subObs(static_cast< ::residualConnectivity::subObs::subObs&&>(other)), radius(other.radius)
		{}
		subObs::subObs(context const& contextObj, boost::shared_array<const vertexState> state)
			: ::residualConnectivity::observation(contextObj, state)
		{}
		subObs& subObs::operator=(const subObs& other)
		{
			observation::operator=(other);
			return *this;
		}
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
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			const std::vector<double>& operationalProbabilitiesD = contextObj.getOperationalProbabilitiesD();
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					boost::random::bernoulli_distribution<double> vertexDistribution(operationalProbabilitiesD[i]);
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

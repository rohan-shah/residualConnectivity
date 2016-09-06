#include "subObs/basic.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
namespace residualConnectivity
{
	namespace subObs
	{
		basic::basic(context const& contextObj, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::basicConstructorType& otherData)
			: ::residualConnectivity::subObs::subObsWithRadius(contextObj, state, radius)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
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
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			const std::vector<double>& operationalProbabilitiesD = contextObj.getOperationalProbabilitiesD();
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

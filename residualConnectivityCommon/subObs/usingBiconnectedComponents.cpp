#include "subObs/usingBiconnectedComponents.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "obs/usingBiconnectedComponents.h"
#include "subObs/withFinalConditioning.hpp"
namespace residualConnectivity
{
	namespace subObs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(context, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
		}
		bool usingBiconnectedComponents::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		usingBiconnectedComponents::usingBiconnectedComponents(usingBiconnectedComponents&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void usingBiconnectedComponents::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
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
			otherData.weight = weight;
		}
		void usingBiconnectedComponents::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			withFinalConditioning::estimateRadius1<usingBiconnectedComponents>(*this, randomSource, nSimulations, scratchMemory, stack, outputObservations);
		}
	}
}

#include "subObs/articulationConditioning.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "obs/articulationConditioning.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioning::articulationConditioning(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected)
			{
				conditionArticulation(state, weight, contextObj, otherData.components, otherData.filteredGraphStack);
			}
		}
		bool articulationConditioning::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioning::articulationConditioning(articulationConditioning&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void articulationConditioning::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
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
			otherData.weight = weight;
		}
		mpfr_class articulationConditioning::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			return withFinalConditioning::estimateRadius1<articulationConditioning>(*this, randomSource, nSimulations, scratchMemory, stack);
		}
		articulationConditioning articulationConditioning::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioning(*this, weight);
		}
		articulationConditioning::articulationConditioning(const articulationConditioning& other, mpfr_class weight)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected)
		{}
		articulationConditioning::articulationConditioning(const articulationConditioning& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected)
		{}
	}
}

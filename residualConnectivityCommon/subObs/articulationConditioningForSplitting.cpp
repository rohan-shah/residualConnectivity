#include "subObs/articulationConditioningForSplitting.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/articulationConditioningForSplitting.h"
#include "obs/articulationConditioningForSplitting.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningForSplitting::articulationConditioningForSplitting(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(radius != 1 && potentiallyConnected)
			{
				conditionArticulation(state, weight, contextObj, otherData.components, otherData.subGraphStack, otherData.subGraph);
			}
		}
		bool articulationConditioningForSplitting::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningForSplitting::articulationConditioningForSplitting(articulationConditioningForSplitting&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void articulationConditioningForSplitting::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
		{
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			const std::vector<double> operationalProbabilityD = contextObj.getOperationalProbabilitiesD();
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					boost::random::bernoulli_distribution<double> vertexDistribution(operationalProbabilityD[i]);
					if(vertexDistribution(randomSource))
					{
						outputState[i].state = UNFIXED_ON;
					}
					else outputState[i].state = UNFIXED_OFF;
				}
			}
			otherData.weight = weight;
		}
		void articulationConditioningForSplitting::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			withFinalConditioning::estimateRadius1<articulationConditioningForSplitting>(*this, randomSource, nSimulations, scratchMemory, stack, outputObservations);
		}
		articulationConditioningForSplitting articulationConditioningForSplitting::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningForSplitting(*this, weight);
		}
		articulationConditioningForSplitting::articulationConditioningForSplitting(const articulationConditioningForSplitting& other, mpfr_class weight)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected)
		{}
		articulationConditioningForSplitting::articulationConditioningForSplitting(const articulationConditioningForSplitting& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected)
		{}
	}
}

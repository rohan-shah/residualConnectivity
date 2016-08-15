#include "subObs/articulationConditioningForResampling.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/articulationConditioningForResampling.h"
#include "obs/articulationConditioningForResampling.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningForResampling::articulationConditioningForResampling(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningForResamplingConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected && otherData.useConditioning)
			{
				conditionArticulation(state, weight, contextObj, otherData.components, otherData.subGraphStack, otherData.subGraph);
			}
		}
		bool articulationConditioningForResampling::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningForResampling::articulationConditioningForResampling(articulationConditioningForResampling&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void articulationConditioningForResampling::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
		{
			std::size_t nVertices = contextObj.nVertices();
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
		articulationConditioningForResampling articulationConditioningForResampling::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningForResampling(*this, weight);
		}
		articulationConditioningForResampling::articulationConditioningForResampling(const articulationConditioningForResampling& other, mpfr_class weight)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected)
		{}
		articulationConditioningForResampling::articulationConditioningForResampling(const articulationConditioningForResampling& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected)
		{}
	}
}

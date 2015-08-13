#include "subObs/articulationConditioningForResampling.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/articulationConditioningForResampling.h"
#include "obs/articulationConditioningForResampling.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		articulationConditioningForResampling::articulationConditioningForResampling(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::articulationConditioningForResamplingConstructorType& otherData)
			: ::discreteGermGrain::subObs::withWeight(context, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected && otherData.useConditioning)
			{
				conditionArticulation(state, weight, context, otherData.components, otherData.subGraphStack, otherData.subGraph);
			}
		}
		bool articulationConditioningForResampling::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningForResampling::articulationConditioningForResampling(articulationConditioningForResampling&& other)
			: ::discreteGermGrain::subObs::withWeight(static_cast< ::discreteGermGrain::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void articulationConditioningForResampling::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
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
		articulationConditioningForResampling articulationConditioningForResampling::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningForResampling(*this, weight);
		}
		articulationConditioningForResampling::articulationConditioningForResampling(const articulationConditioningForResampling& other, mpfr_class weight)
			: ::discreteGermGrain::subObs::withWeight(static_cast<const ::discreteGermGrain::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected)
		{}
		articulationConditioningForResampling::articulationConditioningForResampling(const articulationConditioningForResampling& other)
			: ::discreteGermGrain::subObs::withWeight(static_cast<const ::discreteGermGrain::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected)
		{}
	}
}

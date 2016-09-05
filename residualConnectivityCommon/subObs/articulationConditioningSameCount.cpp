#include "subObs/articulationConditioningSameCount.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/articulationConditioningSameCount.h"
#include "obs/articulationConditioningSameCount.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
#include "findFixedOnVisitor.h"
#include "conditionalPoissonBase.h"
#include "samplingBase.h"
#include <boost/graph/filtered_graph.hpp>
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningSameCount::articulationConditioningSameCount(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningSameCountConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight), nUpVertices(otherData.count)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected && otherData.useConditioning)
			{
				//Pass state here because when it's stored into (this) it's stored as const. 
				this->conditionArticulation(state, otherData.components, otherData.filteredGraphStack);
			}
		}
		void articulationConditioningSameCount::conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& graphVertices, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
		{
			//This is the same as the code in conditionArticulation.cpp. But because we already know the totally number of up vertices the code is slightly different. 
			/*const context::inputGraph& originalGraph = contextObj.getGraph();
			std::size_t nVertices = boost::num_vertices(originalGraph);
		
			//Code related to sampling
			int alreadyFixed = 0;
			sampling::conditionalPoissonArgs samplingArgs;
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(state[i].state == FIXED_ON)
				{
					alreadyFixed++;
					samplingArgs.weights.push_back(1);
				}
				else if(state[i].state == FIXED_OFF)
				{
					samplingArgs.weights.push_back(0);
				}
				else
				{
					samplingArgs.weights.push_back(operationalProbabilities[i]);
				}
			}
			samplingArgs.n = nUpVertices - alreadyFixed;
			int nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase(samplingArgs.n, samplingArgs.indices, samplingArgs.weights, samplingArgs.zeroWeights, samplingArgs.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgs);
			std::vector<sampling::mpfr_class> inclusionProbabilities;
			sampling::conditionalPoissonInclusionProbabilities(samplingArgs, inclusionProbabilities);*/
		}
		bool articulationConditioningSameCount::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningSameCount::articulationConditioningSameCount(articulationConditioningSameCount&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices)
		{}
		void articulationConditioningSameCount::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData) const
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
			otherData.count = nUpVertices;
		}
		articulationConditioningSameCount articulationConditioningSameCount::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningSameCount(*this, weight, nUpVertices);
		}
		articulationConditioningSameCount::articulationConditioningSameCount(const articulationConditioningSameCount& other, mpfr_class weight, int nUpVertices)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected), nUpVertices(nUpVertices)
		{}
		articulationConditioningSameCount::articulationConditioningSameCount(const articulationConditioningSameCount& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices)
		{}
	}
}

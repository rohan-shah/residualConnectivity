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
#include <boost/graph/filtered_graph.hpp>
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningSameCount::articulationConditioningSameCount(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningSameCountConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight), nUpVertices(otherData.count)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected)
			{
				//Pass state here because when it's stored into (this) it's stored as const. 
				this->conditionArticulation(state, otherData.components, otherData.filteredGraphStack);
			}
		}
		void articulationConditioningSameCount::conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& graphVertices, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
		{
			//This is similar to the code in conditionArticulation.cpp. But because we already know the totally number of up vertices the code is slightly different. 
			const context::inputGraph& graph = contextObj.getGraph();
			filteredGraphType filtered(graph, boost::keep_all(), filterByStateMask(state.get(), UNFIXED_MASK | ON_MASK));
			std::size_t nVertices = boost::num_vertices(graph);
			const std::vector<mpfr_class> operationalProbabilities = contextObj.getOperationalProbabilities();
				
			//get out biconnected components of helper graph (which has different vertex ids, remember)
			std::vector<std::size_t> articulationVertices;
			boost::articulation_points(filtered, std::back_inserter(articulationVertices));
		
			//Not all articulation vertices actually *need* to be up in order for the graph to be connected. So check which ones are required. 
			typedef boost::color_traits<boost::default_color_type> Color;
			std::vector<boost::default_color_type> colorMap(boost::num_vertices(graph), Color::white());
			findFixedOnVisitor fixedVisitor(state.get());

			std::vector<int> conditioningVertices;
			for(std::vector<std::size_t>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
			{
				if(state[*i].state != FIXED_ON)
				{
					std::fill(colorMap.begin(), colorMap.end(), Color::white());
					colorMap[*i] = Color::black();

					filteredGraphType::out_edge_iterator current, end;
					boost::tie(current, end) = boost::out_edges(*i, filtered);
					int nComponentsWithFixedOnVertices = 0;
					for(; current != end; current++)
					{
						std::size_t otherVertex = current->m_target;
						if(colorMap[otherVertex] != Color::black())
						{
							fixedVisitor.found = false;
							boost::detail::depth_first_visit_restricted_impl(filtered, otherVertex, fixedVisitor, &(colorMap[0]), filteredGraphStack, boost::detail::nontruth2());
							if(fixedVisitor.found) nComponentsWithFixedOnVertices++;
							if(nComponentsWithFixedOnVertices > 1) break;
						}
					}
					if(nComponentsWithFixedOnVertices > 1)
					{
						state[*i].state = FIXED_ON;
						conditioningVertices.push_back(*i);
					}
				}
			}

			//Order in which the vertices are simulated. The articulation vertices are going to be first, to make it easier to compute the probability that all these vertices are present. 
			simulationOrder.clear();
			simulationOrder.reserve(nVertices);
			for(std::size_t i = 0; i < nVertices; i++) simulationOrder.push_back((int)i);
			std::sort(conditioningVertices.begin(), conditioningVertices.end());
			for(std::size_t i = 0; i < conditioningVertices.size(); i++)
			{
				std::swap(simulationOrder[i], simulationOrder[conditioningVertices[i]]);
			}
			samplingArgs.weights.clear();
			samplingArgs.indices.clear();
			//Put in the probabilities for the conditioning vertices. These have to be handled seperately because they've already been marked as FIXED_ON, and we don't want to put in the value of 1 for the probability - We want to put in the actual probability. 
			for(std::size_t i = 0; i < conditioningVertices.size(); i++)
			{
				samplingArgs.weights.push_back(operationalProbabilities[simulationOrder[i]]);
			}
			int alreadyOn = (int)conditioningVertices.size(), possibleExtra = 0;
			for(std::size_t i = (int)conditioningVertices.size(); i < nVertices; i++)
			{
				if(state[simulationOrder[i]].state == FIXED_ON)
				{
					samplingArgs.weights.push_back(1);
					alreadyOn++;
				}
				else if(state[simulationOrder[i]].state == FIXED_OFF)
				{
					samplingArgs.weights.push_back(0);
				}
				else
				{
					samplingArgs.weights.push_back(operationalProbabilities[simulationOrder[i]]);
					if(operationalProbabilities[simulationOrder[i]] != 0) possibleExtra++;
				}
			}
			if(alreadyOn > nUpVertices || alreadyOn + possibleExtra < nUpVertices)
			{
				potentiallyConnected = false;
				return;
			}
			samplingArgs.n = nUpVertices;
			int nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase(samplingArgs.n, samplingArgs.indices, samplingArgs.weights, samplingArgs.zeroWeights, samplingArgs.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgs);
			calculateExpNormalisingConstants(samplingArgs);


			//We need to work out the probability of sampling the conditioning vertices. 
			//The following is a modification of the code in conditionalPoissonSequential.cpp
			int chosen = 0;
			int skipped = 0;
			for(int i = 0; i < (int)conditioningVertices.size(); i++)
			{
				if(!samplingArgs.zeroWeights[i] && !samplingArgs.deterministicInclusion[i])
				{
					if(samplingArgs.n - nDeterministic - 1 - chosen == 0)
					{
						weight *= (samplingArgs.expExponentialParameters[i] / samplingArgs.expNormalisingConstant(i-skipped, samplingArgs.n - nDeterministic - chosen - 1));
					}
					else
					{
						weight *= (samplingArgs.expExponentialParameters[i] * samplingArgs.expNormalisingConstant(i+1-skipped, samplingArgs.n - nDeterministic - 1 - chosen - 1) / samplingArgs.expNormalisingConstant(i-skipped, samplingArgs.n - nDeterministic - chosen - 1));
					}
					chosen++;
				}
				else skipped++;
				if(chosen == (int)samplingArgs.n - nDeterministic) break;
			}
			nConditioningVertices = (int)conditioningVertices.size();
		}
		bool articulationConditioningSameCount::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningSameCount::articulationConditioningSameCount(articulationConditioningSameCount&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), simulationOrder(std::move(other.simulationOrder)), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices), samplingArgs(std::move(other.samplingArgs)), nConditioningVertices(other.nConditioningVertices)
		{}
		void articulationConditioningSameCount::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData) const
		{
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			
			int nDeterministic = 0, nZeroWeights = 0;
			for(int i = 0; i < (int)nVertices; i++)
			{
				if(samplingArgs.zeroWeights[i]) nZeroWeights++;
				if(samplingArgs.deterministicInclusion[i]) nDeterministic++;
			}
			int chosen = nConditioningVertices;
			int skipped = 0;
			if((int)samplingArgs.n != nDeterministic + chosen)
			{
				for(int i = nConditioningVertices; i < (int)simulationOrder.size(); i++)
				{
					if(!samplingArgs.zeroWeights[i] && !samplingArgs.deterministicInclusion[i])
					{
						double parameter;
						if(samplingArgs.n - nDeterministic - 1 - chosen == 0)
						{
							parameter = mpfr_class(samplingArgs.expExponentialParameters[i] / samplingArgs.expNormalisingConstant(i-skipped, samplingArgs.n - nDeterministic - chosen - 1)).convert_to<double>();
						}
						else
						{
							parameter = mpfr_class(samplingArgs.expExponentialParameters[i] * samplingArgs.expNormalisingConstant(i+1-skipped, samplingArgs.n - nDeterministic - 1 - chosen - 1) / samplingArgs.expNormalisingConstant(i-skipped, samplingArgs.n - nDeterministic - chosen - 1)).convert_to<double>();
						}
						//Vertices are only marked as UNFIXED_ON or UNFIXED_OFF, because those marked as FIXED_ON or FIXED_OFF are excluded by th zeroWeights / deterministicInclusion conditions. And those that have just been conditioned on are excluded by the i = nConditioningVertices. 
						boost::random::bernoulli_distribution<> bernoulli(parameter);
						if(bernoulli(randomSource))
						{
							outputState[simulationOrder[i]].state = UNFIXED_ON;
							chosen++;
						}
						else outputState[simulationOrder[i]].state = UNFIXED_OFF;
					}
					else skipped++;
					if(chosen == (int)samplingArgs.n - nDeterministic) break;
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
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), simulationOrder(other.simulationOrder), potentiallyConnected(other.potentiallyConnected), nUpVertices(nUpVertices), nConditioningVertices(other.nConditioningVertices)
		{
			samplingArgs.n = other.samplingArgs.n;
			samplingArgs.exponentialParameters = other.samplingArgs.exponentialParameters;
			samplingArgs.weights = other.samplingArgs.weights;
			samplingArgs.expExponentialParameters = other.samplingArgs.expExponentialParameters;
			samplingArgs.expNormalisingConstant = other.samplingArgs.expNormalisingConstant;
			samplingArgs.deterministicInclusion = other.samplingArgs.deterministicInclusion;
			samplingArgs.zeroWeights = other.samplingArgs.zeroWeights;
		}
		articulationConditioningSameCount::articulationConditioningSameCount(const articulationConditioningSameCount& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), simulationOrder(other.simulationOrder), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices), nConditioningVertices(other.nConditioningVertices)
		{
			samplingArgs.n = other.samplingArgs.n;
			samplingArgs.exponentialParameters = other.samplingArgs.exponentialParameters;
			samplingArgs.weights = other.samplingArgs.weights;
			samplingArgs.expExponentialParameters = other.samplingArgs.expExponentialParameters;
			samplingArgs.expNormalisingConstant = other.samplingArgs.expNormalisingConstant;
			samplingArgs.deterministicInclusion = other.samplingArgs.deterministicInclusion;
			samplingArgs.zeroWeights = other.samplingArgs.zeroWeights;
		}
	}
}

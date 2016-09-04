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
				this->conditionArticulation(state, otherData.components, otherData.subGraphStack, otherData.subGraph);
			}
		}
		void articulationConditioningSameCount::conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& graphVertices, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& graph)
		{
			//This is the same as the code in conditionArticulation.cpp. But because we already know the totally number of up vertices the code is slightly different. 
			
			//construct subgraph
			graph.clear();
			//Reuse components vector here
			{
				constructSubGraph(graph, graphVertices, contextObj, state.get());
				//assign edge indices
				subGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}
			//get out biconnected components of helper graph (which has different vertex ids, remember)
			std::vector<std::size_t> articulationVertices;
			boost::articulation_points(graph, std::back_inserter(articulationVertices));
		
			typedef boost::color_traits<boost::default_color_type> Color;
			std::vector<boost::default_color_type> colorMap(boost::num_vertices(graph), Color::white());
			findFixedOnVisitor fixedVisitor(state.get(), graphVertices);
			const std::vector<mpfr_class> operationalProbabilities = contextObj.getOperationalProbabilities();

			//Code related to sampling
			int alreadyFixed = 0;
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
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
			sampling::conditionalPoissonInclusionProbabilities(samplingArgs, inclusionProbabilities);

			for(std::vector<std::size_t>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
			{
				if(state[graphVertices[*i]].state != FIXED_ON)
				{
					std::fill(colorMap.begin(), colorMap.end(), Color::white());
					colorMap[*i] = Color::black();

					subGraphType::out_edge_iterator current, end;
					boost::tie(current, end) = boost::out_edges(*i, graph);
					int nComponentsWithFixedOnVertices = 0;
					for(; current != end; current++)
					{
						std::size_t otherVertex = current->m_target;
						if(colorMap[otherVertex] != Color::black())
						{
							fixedVisitor.found = false;
							boost::detail::depth_first_visit_restricted_impl(graph, otherVertex, fixedVisitor, &(colorMap[0]), subGraphStack, boost::detail::nontruth2());
							if(fixedVisitor.found) nComponentsWithFixedOnVertices++;
							if(nComponentsWithFixedOnVertices > 1) break;
						}
					}
					if(nComponentsWithFixedOnVertices > 1)
					{
						state[graphVertices[*i]] = vertexState::fixed_on();
						weight *= inclusionProbabilities[graphVertices[*i]];
					}
				}
			}
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

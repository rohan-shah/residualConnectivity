#include "subObs/articulationConditioningSameCountImportance.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "obs/articulationConditioningSameCountImportance.h"
#include "constructSubGraph.h"
#include "findFixedOnVisitor.h"
#include <boost/graph/filtered_graph.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/scoped_array.hpp>
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningSameCountImportanceConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight), nUpVertices(otherData.count), importanceProbabilities(otherData.importanceProbabilities)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected)
			{
				//Pass state here because when it's stored into (this) it's stored as const. 
				this->conditionArticulation(state, otherData.components, otherData.filteredGraphStack);
			}
		}
		void articulationConditioningSameCountImportance::conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& graphVertices, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack)
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
						conditioningVertices.push_back((int)*i);
					}
				}
			}

			//Order in which the vertices are simulated. The articulation vertices are going to be first, to make it easier to compute the probability that all these vertices are present. 
			this->simulationOrder.reset(new std::vector<int>());
			std::vector<int>& simulationOrder = *this->simulationOrder.get();
			simulationOrder.clear();
			simulationOrder.reserve(nVertices);
			for(std::size_t i = 0; i < nVertices; i++) simulationOrder.push_back((int)i);
			std::sort(conditioningVertices.begin(), conditioningVertices.end());
			for(std::size_t i = 0; i < conditioningVertices.size(); i++)
			{
				std::swap(simulationOrder[i], simulationOrder[conditioningVertices[i]]);
			}
			this->samplingArgsOriginal.reset(new sampling::conditionalPoissonArgs());
			sampling::conditionalPoissonArgs& samplingArgsOriginal = *this->samplingArgsOriginal.get();
			samplingArgsOriginal.weights.clear();
			samplingArgsOriginal.indices.clear();
			//Put in the probabilities for the conditioning vertices. These have to be handled seperately because they've already been marked as FIXED_ON, and we don't want to put in the value of 1 for the probability - We want to put in the actual probability. 
			for(std::size_t i = 0; i < conditioningVertices.size(); i++)
			{
				samplingArgsOriginal.weights.push_back(operationalProbabilities[simulationOrder[i]]);
			}
			int alreadyOn = (int)conditioningVertices.size(), possibleExtra = 0;
			for(std::size_t i = conditioningVertices.size(); i < nVertices; i++)
			{
				if(state[simulationOrder[i]].state == FIXED_ON)
				{
					samplingArgsOriginal.weights.push_back(1);
					alreadyOn++;
				}
				else if(state[simulationOrder[i]].state == FIXED_OFF)
				{
					samplingArgsOriginal.weights.push_back(0);
				}
				else
				{
					samplingArgsOriginal.weights.push_back(operationalProbabilities[simulationOrder[i]]);
					if(operationalProbabilities[simulationOrder[i]] != 0) possibleExtra++;
				}
			}
			if(alreadyOn > nUpVertices || alreadyOn + possibleExtra < nUpVertices)
			{
				potentiallyConnected = false;
				return;
			}
			samplingArgsOriginal.n = nUpVertices;
			int nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase((int)samplingArgsOriginal.n, samplingArgsOriginal.indices, samplingArgsOriginal.weights, samplingArgsOriginal.zeroWeights, samplingArgsOriginal.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgsOriginal);
			calculateExpNormalisingConstants(samplingArgsOriginal);

			this->samplingArgsImportance.reset(new sampling::conditionalPoissonArgs());
			sampling::conditionalPoissonArgs& samplingArgsImportance = *this->samplingArgsImportance.get();
			samplingArgsImportance.weights.clear();
			samplingArgsImportance.indices.clear();
			for(std::size_t i = 0; i < conditioningVertices.size(); i++)
			{
				samplingArgsImportance.weights.push_back((*importanceProbabilities)[simulationOrder[i]]);
			}
			for(std::size_t i = conditioningVertices.size(); i < nVertices; i++)
			{
				if(state[simulationOrder[i]].state == FIXED_ON)
				{
					samplingArgsImportance.weights.push_back(1);
				}
				else if(state[simulationOrder[i]].state == FIXED_OFF)
				{
					samplingArgsImportance.weights.push_back(0);
				}
				else
				{
					samplingArgsImportance.weights.push_back((*importanceProbabilities)[simulationOrder[i]]);
				}
			}
			samplingArgsImportance.n = nUpVertices;
			nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase((int)samplingArgsImportance.n, samplingArgsImportance.indices, samplingArgsImportance.weights, samplingArgsImportance.zeroWeights, samplingArgsImportance.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgsImportance);
			calculateExpNormalisingConstants(samplingArgsImportance);

			//We need to work out the probability of sampling the conditioning vertices. 
			//The following is a modification of the code in conditionalPoissonSequential.cpp
			int chosen = 0;
			int skipped = 0;
			for(int i = 0; i < (int)conditioningVertices.size(); i++)
			{
				if(!samplingArgsOriginal.zeroWeights[i] && !samplingArgsOriginal.deterministicInclusion[i])
				{
					if(samplingArgsOriginal.n - nDeterministic - 1 - chosen == 0)
					{
						weight *= (samplingArgsOriginal.expExponentialParameters[i] / samplingArgsOriginal.expNormalisingConstant(i-skipped, samplingArgsOriginal.n - nDeterministic - chosen - 1));// / (samplingArgsImportance.expExponentialParameters[i] / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1));
					}
					else
					{
						weight *= ((samplingArgsOriginal.expExponentialParameters[i] * samplingArgsOriginal.expNormalisingConstant(i+1-skipped, samplingArgsOriginal.n - nDeterministic - 1 - chosen - 1) / samplingArgsOriginal.expNormalisingConstant(i-skipped, samplingArgsOriginal.n - nDeterministic - chosen - 1)));// / ((samplingArgsImportance.expExponentialParameters[i] * samplingArgsImportance.expNormalisingConstant(i+1-skipped, samplingArgsImportance.n - nDeterministic - 1 - chosen - 1) / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1)));
					}
					chosen++;
				}
				else skipped++;
				if(chosen == (int)samplingArgsOriginal.n - nDeterministic) break;
			}
			nConditioningVertices = (int)conditioningVertices.size();
		}
		bool articulationConditioningSameCountImportance::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(articulationConditioningSameCountImportance&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), simulationOrder(std::move(other.simulationOrder)), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices), samplingArgsOriginal(std::move(other.samplingArgsOriginal)), samplingArgsImportance(std::move(other.samplingArgsImportance)), nConditioningVertices(other.nConditioningVertices), importanceProbabilities(other.importanceProbabilities)
		{}
		void articulationConditioningSameCountImportance::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData) const
		{
			sampling::conditionalPoissonArgs& samplingArgsImportance = *this->samplingArgsImportance.get();
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			
			int nDeterministic = 0, nZeroWeights = 0;
			for(int i = 0; i < (int)nVertices; i++)
			{
				if(samplingArgsImportance.zeroWeights[i]) nZeroWeights++;
				if(samplingArgsImportance.deterministicInclusion[i]) nDeterministic++;
			}
			int chosen = nConditioningVertices;
			int skipped = 0;
			if((int)samplingArgsImportance.n != nDeterministic + chosen)
			{
				std::vector<int>& simulationOrder = *this->simulationOrder.get();
				for(int i = nConditioningVertices; i < (int)simulationOrder.size(); i++)
				{
					if(!samplingArgsImportance.zeroWeights[i] && !samplingArgsImportance.deterministicInclusion[i])
					{
						double parameter;
						if(samplingArgsImportance.n - nDeterministic - 1 - chosen == 0)
						{
							parameter = mpfr_class(samplingArgsImportance.expExponentialParameters[i] / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1)).convert_to<double>();
						}
						else
						{
							parameter = mpfr_class(samplingArgsImportance.expExponentialParameters[i] * samplingArgsImportance.expNormalisingConstant(i+1-skipped, samplingArgsImportance.n - nDeterministic - 1 - chosen - 1) / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1)).convert_to<double>();
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
					if(chosen == (int)samplingArgsImportance.n - nDeterministic) break;
				}
			}
			otherData.weight = weight;
			otherData.count = nUpVertices;
			otherData.importanceProbabilities = importanceProbabilities;
		}
		articulationConditioningSameCountImportance articulationConditioningSameCountImportance::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningSameCountImportance(*this, weight, nUpVertices, importanceProbabilities);
		}
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(const articulationConditioningSameCountImportance& other, mpfr_class weight, int nUpVertices, const std::vector<double>* importanceProbabilities)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), simulationOrder(other.simulationOrder), potentiallyConnected(other.potentiallyConnected), nUpVertices(nUpVertices), samplingArgsOriginal(other.samplingArgsOriginal), samplingArgsImportance(other.samplingArgsImportance), nConditioningVertices(other.nConditioningVertices), importanceProbabilities(importanceProbabilities)
		{}
		articulationConditioningSameCountImportance& articulationConditioningSameCountImportance::operator=(const articulationConditioningSameCountImportance& other)
		{
			::residualConnectivity::subObs::withWeight::operator=(other);
			simulationOrder = other.simulationOrder;
			potentiallyConnected = other.potentiallyConnected;
			nUpVertices = other.nUpVertices;
			samplingArgsOriginal = other.samplingArgsOriginal;
			samplingArgsImportance = other.samplingArgsImportance;
			nConditioningVertices = other.nConditioningVertices;
			importanceProbabilities = other.importanceProbabilities;
			return *this;
		}
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(const articulationConditioningSameCountImportance& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), simulationOrder(other.simulationOrder), potentiallyConnected(other.potentiallyConnected), nUpVertices(other.nUpVertices), samplingArgsOriginal(other.samplingArgsOriginal), samplingArgsImportance(other.samplingArgsImportance), nConditioningVertices(other.nConditioningVertices), importanceProbabilities(other.importanceProbabilities)
		{}
		mpfr_class articulationConditioningSameCountImportance::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			if(nSimulations <= 0)
			{
				throw std::runtime_error("Input nSimulations must be positive");
			}
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call estimateRadius1");
			}
			//Construct helper graph, containing everything except FIXED_OFF vertices. This is because in order to compute the biconnected components / articulation vertices, we need an actual graph object, not just a state vector
			subGraphType graph;
			const vertexState* stateRef = state.get();
			std::vector<int> graphVertices;
			{
				constructSubGraph(graph, graphVertices, contextObj, stateRef);
				//assign edge indices
				subGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}
			//If there are no unfixed vertices we can return immediately.
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			std::vector<context::inputGraph::vertex_descriptor> unfixedVertices;
			int fixedOnVertices = 0;
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(stateRef[i].state & UNFIXED_MASK)
				{
					unfixedVertices.push_back(i);
				}
				else if(stateRef[i].state == FIXED_ON)
				{
					fixedOnVertices++;
				}
			}
			if(unfixedVertices.size() == 0)
			{
				if(potentiallyConnected)
				{
					return 1;
				}
				return 0;
			}
			else if(fixedOnVertices == nUpVertices)
			{
				boost::scoped_array<vertexState> copiedState(new vertexState[nVertices]);
				memcpy(copiedState.get(), stateRef, sizeof(vertexState)*nVertices);
				//Mark everything that's not fixed as OFF
				for(std::size_t i = 0; i < nVertices; i++)
				{
					if(stateRef[i].state & UNFIXED_MASK) copiedState[i].state = FIXED_OFF;
				}
				if(isSingleComponentPossible(contextObj.getGraph(), copiedState.get(), scratchMemory, stack)) return 1;
				return 0;
			}

			//get out biconnected components of helper graph (which has different vertex ids, remember)
			typedef std::vector<boost::graph_traits<subGraphType>::edges_size_type> component_storage_t;
			typedef boost::iterator_property_map<component_storage_t::iterator, boost::property_map<subGraphType, boost::edge_index_t>::type> component_map_t;
			component_storage_t biconnectedIds(boost::num_edges(graph));
			component_map_t componentMap(biconnectedIds.begin(), boost::get(boost::edge_index, graph));
			std::vector<std::size_t> articulationVertices;
			boost::biconnected_components(graph, componentMap, std::back_inserter(articulationVertices));

			//Count the number of biconnected components. I believe these are guaranteed to be contiguous....
			std::size_t nBiconnectedComponents = *std::max_element(biconnectedIds.begin(), biconnectedIds.end())+1;
			//allocate a set per biconnected component
			std::vector<std::set<int> > biConnectedSets(nBiconnectedComponents);
			//Add to each set the indices of the vertices that are contained in this biconnected component
			{
				subGraphType::edge_iterator start, end;
				boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++)
				{
					std::set<int>& currentComponentSet = biConnectedSets[biconnectedIds[boost::get(edgePropertyMap, *start)]];
					currentComponentSet.insert((int)graphVertices[start->m_source]);
					currentComponentSet.insert((int)graphVertices[start->m_target]);
				}
			}
			//vectors containing the fixed and unfixed vertices for each component
			std::vector<std::vector<context::inputGraph::vertex_descriptor> > fixedPointsPerComponent;
			std::vector<std::vector<context::inputGraph::vertex_descriptor> > unFixedPointsPerComponent;
			//This is the simulation order for the splitting algorithm. We only need to simulate the unfixed points. It's safe to just aggregate the unfixed points from all the biconnected components, because any vertex that lies in more than one connected component is an articulation vertex, and has therefore already been marked as FIXED_ON. 
			std::vector<int> newSimulationOrder;
			for(std::size_t componentCounter = 0; componentCounter < nBiconnectedComponents; componentCounter++)
			{
				std::set<int>& currentComponentVertices = biConnectedSets[componentCounter];
				std::vector<context::inputGraph::vertex_descriptor> currentComponentFixed, currentComponentUnFixed;
				for(std::set<int>::iterator vertexIterator = currentComponentVertices.begin(); vertexIterator != currentComponentVertices.end(); vertexIterator++)
				{
					if((stateRef[*vertexIterator].state & FIXED_MASK)) currentComponentFixed.push_back(*vertexIterator);
					else 
					{
						currentComponentUnFixed.push_back(*vertexIterator);
						newSimulationOrder.push_back(*vertexIterator);
					}
				}
				fixedPointsPerComponent.push_back(std::move(currentComponentFixed));
				unFixedPointsPerComponent.push_back(std::move(currentComponentUnFixed));
			}
			//Set up sampling code, for sampling from JUST the unfixed vertices. In the order specified by newSimulationOrder. 
			sampling::conditionalPoissonArgs newSamplingArgsOriginal, newSamplingArgsImportance;
			newSamplingArgsOriginal.n = newSamplingArgsImportance.n = nUpVertices - fixedOnVertices;
			int nDeterministic = 0, nZeroWeights = 0;
			const std::vector<mpfr_class> operationalProbabilities = contextObj.getOperationalProbabilities();
			for(std::vector<int>::iterator i = newSimulationOrder.begin(); i != newSimulationOrder.end(); i++) newSamplingArgsOriginal.weights.push_back(operationalProbabilities[*i]);
			sampling::samplingBase((int)newSamplingArgsOriginal.n, newSamplingArgsOriginal.indices, newSamplingArgsOriginal.weights, newSamplingArgsOriginal.zeroWeights, newSamplingArgsOriginal.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(newSamplingArgsOriginal);
			calculateExpNormalisingConstants(newSamplingArgsOriginal);

			for(std::vector<int>::iterator i = newSimulationOrder.begin(); i != newSimulationOrder.end(); i++) newSamplingArgsImportance.weights.push_back((*importanceProbabilities)[*i]);
			nDeterministic = 0;
			nZeroWeights = 0;
			sampling::samplingBase((int)newSamplingArgsImportance.n, newSamplingArgsImportance.indices, newSamplingArgsImportance.weights, newSamplingArgsImportance.zeroWeights, newSamplingArgsImportance.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(newSamplingArgsImportance);
			calculateExpNormalisingConstants(newSamplingArgsImportance);
			
			std::vector<mpfr_class> weights;
			std::vector<int> nChosen(nSimulations, 0);
			std::vector<int> otherNChosen;
			int cumulativeSimulationCount = 0;
			mpfr_class estimate = 1;
			boost::shared_array<vertexState> simulatedState(new vertexState[nVertices]);
			int skipped = 0;
			//Iterate over biconnected components
			for(int i = 0; i < (int)nBiconnectedComponents; i++)
			{
				//Copy in the current state
				memcpy(simulatedState.get(), state.get(), sizeof(vertexState)*nVertices);
				otherNChosen.clear();
				weights.clear();
				//Number of vertices that are being simulated in this biconnected component
				std::vector<context::inputGraph::vertex_descriptor>& unFixedVerticesThisComponent = unFixedPointsPerComponent[i];
				std::vector<context::inputGraph::vertex_descriptor>& fixedVerticesThisComponent = fixedPointsPerComponent[i];
				int currentComponentSize = (int)unFixedVerticesThisComponent.size();
				int copiedSkipped = skipped;
				//Loop over all the particles
				for(int j = 0; j < nSimulations; j++)
				{
					//Values of skipped are the same for all particles. 
					skipped = copiedSkipped;
					//Number of vertices in this biconnected component, for this simulation, which are selected as being present. 
					int chosenCurrentComponent = 0;
					mpfr_class currentWeight = 1;
					//Simulate the values for this biconnected component, for this particle. 
					for(int k = 0; k < currentComponentSize; k++)
					{
						if(nChosen[j] + chosenCurrentComponent == (int)newSamplingArgsOriginal.n - nDeterministic) simulatedState[newSimulationOrder[k+cumulativeSimulationCount]] = vertexState::unfixed_off();
						else if(!newSamplingArgsOriginal.zeroWeights[cumulativeSimulationCount + k] && !newSamplingArgsOriginal.deterministicInclusion[cumulativeSimulationCount + k])
						{
							double parameterOriginal, parameterImportance;
							if(newSamplingArgsOriginal.n - nDeterministic - (nChosen[j] + chosenCurrentComponent) == 1)
							{
								parameterOriginal = mpfr_class(newSamplingArgsOriginal.expExponentialParameters[cumulativeSimulationCount + k] / newSamplingArgsOriginal.expNormalisingConstant(cumulativeSimulationCount + k - skipped, newSamplingArgsOriginal.n - nDeterministic - (nChosen[j] + chosenCurrentComponent) - 1)).convert_to<double>();
								parameterImportance = mpfr_class(newSamplingArgsImportance.expExponentialParameters[cumulativeSimulationCount + k] / newSamplingArgsImportance.expNormalisingConstant(cumulativeSimulationCount + k - skipped, newSamplingArgsImportance.n - nDeterministic - (nChosen[j] + chosenCurrentComponent) - 1)).convert_to<double>();
							}
							else
							{
								parameterOriginal = mpfr_class(newSamplingArgsOriginal.expExponentialParameters[cumulativeSimulationCount + k] * newSamplingArgsOriginal.expNormalisingConstant(cumulativeSimulationCount + k+1-skipped, newSamplingArgsOriginal.n - nDeterministic - 1 - (nChosen[j] + chosenCurrentComponent) - 1) / newSamplingArgsOriginal.expNormalisingConstant(cumulativeSimulationCount + k -skipped, newSamplingArgsOriginal.n - nDeterministic - (nChosen[j] + chosenCurrentComponent) - 1)).convert_to<double>();
								parameterImportance = mpfr_class(newSamplingArgsImportance.expExponentialParameters[cumulativeSimulationCount + k] * newSamplingArgsImportance.expNormalisingConstant(cumulativeSimulationCount + k+1-skipped, newSamplingArgsImportance.n - nDeterministic - 1 - (nChosen[j] + chosenCurrentComponent) - 1) / newSamplingArgsImportance.expNormalisingConstant(cumulativeSimulationCount + k -skipped, newSamplingArgsImportance.n - nDeterministic - (nChosen[j] + chosenCurrentComponent) - 1)).convert_to<double>();
							}
							boost::random::bernoulli_distribution<> bernoulli(parameterImportance);
							if(bernoulli(randomSource))
							{
								chosenCurrentComponent++;
								simulatedState[newSimulationOrder[k+cumulativeSimulationCount]] = vertexState::unfixed_on();
								currentWeight *= parameterOriginal / parameterImportance;
							}
							else
							{
								simulatedState[newSimulationOrder[k+cumulativeSimulationCount]] = vertexState::unfixed_off();
								currentWeight *= (1 - parameterOriginal) / (1 - parameterImportance);
							}
						}
						else skipped++;
					}
					//Retain particles for which the current biconnected component is connected
					if(partIsSingleComponent(contextObj, simulatedState.get(), fixedVerticesThisComponent, unFixedVerticesThisComponent, scratchMemory, stack))
					{
						otherNChosen.push_back(nChosen[j] + chosenCurrentComponent);
						weights.push_back(currentWeight);
					}
				}
				if(otherNChosen.size() == 0) return 0;
				mpfr_class sumWeights = 0;
				for(std::vector<mpfr_class>::iterator ii = weights.begin(); ii != weights.end(); ii++) sumWeights += *ii;
				//Update estimate by the fraction of particles for which the current component was connected. 
				estimate *= (double)sumWeights / (double)nSimulations;
				//Stratified sampling to bring the number of samples up to nSimulations
				int multiple = nSimulations / (int)otherNChosen.size();
				nChosen.clear();
				for(int j = 0; j < multiple; j++)
				{
					nChosen.insert(nChosen.end(), otherNChosen.begin(), otherNChosen.end());
				}
				if((int)nChosen.size() != nSimulations)
				{
					std::vector<int> indices(otherNChosen.size());
					for(int i = 0; i < (int)otherNChosen.size(); i++) indices[i] = i;
					boost::random_shuffle(indices);
					int extra = nSimulations - (int)nChosen.size();
					for(int i = 0; i < extra; i++)
					{
						nChosen.push_back(otherNChosen[indices[i]]);
					}
				}
				//Update the counter tracking where we start sampling at the next iteration
				cumulativeSimulationCount += currentComponentSize;
			}
			return estimate;
		}
	}
}

#include "subObs/articulationConditioningImportance.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/articulationConditioningImportance.h"
#include "obs/articulationConditioningImportance.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace residualConnectivity
{
	namespace subObs
	{
		articulationConditioningImportance::articulationConditioningImportance(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::withWeightImportanceConstructorType& otherData)
			: ::residualConnectivity::subObs::withWeight(contextObj, state, radius, otherData.weight), importanceProbabilities(otherData.importanceProbabilities)
		{
			potentiallyConnected = isSingleComponentPossible(contextObj.getGraph(), state.get(), otherData.components, otherData.stack);
			if(potentiallyConnected)
			{
				conditionArticulation(state, weight, contextObj, otherData.components, otherData.filteredGraphStack);
			}
		}
		bool articulationConditioningImportance::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		articulationConditioningImportance::articulationConditioningImportance(articulationConditioningImportance&& other)
			: ::residualConnectivity::subObs::withWeight(static_cast< ::residualConnectivity::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected), importanceProbabilities(other.importanceProbabilities)
		{
		}
		void articulationConditioningImportance::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData) const
		{
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					boost::random::bernoulli_distribution<double> vertexDistribution((*importanceProbabilities)[i]);
					if(vertexDistribution(randomSource))
					{
						outputState[i].state = UNFIXED_ON;
					}
					else outputState[i].state = UNFIXED_OFF;
				}
			}
			otherData.weight = weight;
			otherData.importanceProbabilities = importanceProbabilities;
		}
		articulationConditioningImportance articulationConditioningImportance::copyWithWeight(mpfr_class weight) const
		{
			return articulationConditioningImportance(*this, weight);
		}
		articulationConditioningImportance& articulationConditioningImportance::operator=(const articulationConditioningImportance& other)
		{
			::residualConnectivity::subObs::withWeight::operator=(other);
			potentiallyConnected = other.potentiallyConnected;
			importanceProbabilities = other.importanceProbabilities;
			return *this;
		}
		articulationConditioningImportance::articulationConditioningImportance(const articulationConditioningImportance& other, mpfr_class weight)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected), importanceProbabilities(other.importanceProbabilities)
		{}
		articulationConditioningImportance::articulationConditioningImportance(const articulationConditioningImportance& other)
			: ::residualConnectivity::subObs::withWeight(static_cast<const ::residualConnectivity::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected), importanceProbabilities(other.importanceProbabilities)
		{}
		mpfr_class articulationConditioningImportance::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
		{
			if(nSimulations <= 0)
			{
				throw std::runtime_error("Input nSimulations must be positive");
			}
			if(getRadius() != 1)
			{
				throw std::runtime_error("Radius must be 1 to call estimateRadius1");
			}
			//Construct helper graph, containing everything except FIXED_OFF vertices. This is because in order to compute the biconnected components / articulation vertices, we need an actual graph object, not just a state vector
			subGraphType graph;
			std::vector<int> graphVertices;
			{
				constructSubGraph(graph, graphVertices, contextObj, getState());
				//assign edge indices
				subGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}
			// Determine which vertices are fixed on, and which are unfixed
			const vertexState* stateRef = getState();
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			std::vector<context::inputGraph::vertex_descriptor> unfixedVertices;
			std::vector<context::inputGraph::vertex_descriptor> fixedOnVertices;
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(stateRef[i].state & UNFIXED_MASK)
				{
					unfixedVertices.push_back(i);
				}
				if(stateRef[i].state &FIXED_ON)
				{
					fixedOnVertices.push_back(i);
				}
			}
			//If there are no unfixed vertices then the graphs at the next step are either always connected or always disconnected
			if(unfixedVertices.size() == 0)
			{
				if(isPotentiallyConnected())
				{
					return 1;
				}
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
			//we're also going to want to work out the number of fixed points per each component, because we're going to append on the unFixedPoints which end up being kept
			//to the end of the fixed points vector (just to save on memory allocations). 
			std::vector<int> nFixedPointsPerComponent;
			for(std::size_t componentCounter = 0; componentCounter < nBiconnectedComponents; componentCounter++)
			{
				std::set<int>& currentComponentVertices = biConnectedSets[componentCounter];
				std::vector<context::inputGraph::vertex_descriptor> currentComponentFixed, currentComponentUnFixed;
				for(std::set<int>::iterator vertexIterator = currentComponentVertices.begin(); vertexIterator != currentComponentVertices.end(); vertexIterator++)
				{
					if((stateRef[*vertexIterator].state & FIXED_MASK)) currentComponentFixed.push_back(*vertexIterator);
					else currentComponentUnFixed.push_back(*vertexIterator);
				}
				nFixedPointsPerComponent.push_back((int)currentComponentFixed.size());
				fixedPointsPerComponent.push_back(std::move(currentComponentFixed));
				unFixedPointsPerComponent.push_back(std::move(currentComponentUnFixed));
			}
			//connectedIndices[0] gives the indices of the entries in generatedObservations that are connected for biconnected component 0
			std::vector<std::vector<int> > connectedIndices(nBiconnectedComponents);
			boost::shared_array<vertexState> obsState(new vertexState[nVertices]);
			std::vector<mpfr_class> componentProbabilities(nBiconnectedComponents, 0);
			const std::vector<mpfr_class>& operationalProbabilities = contextObj.getOperationalProbabilities();

			::residualConnectivity::obs::withWeightImportanceConstructorType getObservationHelper;
			for(int i = 0; i < nSimulations; i++)
			{
				getObservation(obsState.get(), randomSource, getObservationHelper);
				for(std::size_t j = 0; j < nBiconnectedComponents; j++)
				{
					std::vector<context::inputGraph::vertex_descriptor>& currentComponentFixed = fixedPointsPerComponent[j], currentComponentUnFixed = unFixedPointsPerComponent[j];
					mpfr_class currentWeight = 1;
					if(partIsSingleComponent(contextObj, obsState.get(), currentComponentFixed, currentComponentUnFixed, scratchMemory, stack))
					{
						for(std::size_t k = 0; k < currentComponentUnFixed.size(); k++)
						{
							if(obsState[currentComponentUnFixed[k]].state & ON_MASK)
							{
								currentWeight *= operationalProbabilities[currentComponentUnFixed[k]] / (*importanceProbabilities)[currentComponentUnFixed[k]];
							}
							else
							{
								currentWeight *= (1 - operationalProbabilities[currentComponentUnFixed[k]]) / (1 - (*importanceProbabilities)[currentComponentUnFixed[k]]);
							}
						}
						componentProbabilities[j] += currentWeight;
					}
				}
			}
			mpfr_class product = 1;
			for(std::size_t j = 0; j < nBiconnectedComponents; j++) product *= componentProbabilities[j] / nSimulations;
			return product;
		}
	}
}

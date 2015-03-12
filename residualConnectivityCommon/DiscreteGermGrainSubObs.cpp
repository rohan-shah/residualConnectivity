#include "DiscreteGermGrainSubObs.h"
#include "DiscreteGermGrainObs.h"
#include "lexographicOrder.h"
#include <boost/random/bernoulli_distribution.hpp>
#include "isSingleComponentWithRadius.h"
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/property_map/property_map_iterator.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include "bridges.hpp"
namespace discreteGermGrain
{
	DiscreteGermGrainSubObs::DiscreteGermGrainSubObs(Context const& context, boost::shared_array<const vertexState> state, int currentRadius)
		:context(context), currentRadius(currentRadius), state(state)
	{
	}
	void DiscreteGermGrainSubObs::getSingleComponentObservation(int newRadius, boost::mt19937& randomSource, std::vector<int>& componentIDs, DiscreteGermGrainObs& outputObservation, float& outputProbability)
	{
		/*if(newRadius == 0)
		{
			outputObservation = estimateRadius1(randomSource, -1, outputProbability);
			return;
		}
		graph_t graph;
		constructGraphFromRadius(currentRadius, graph);
		//assign edge indices
		{
			graph_t::edge_iterator start, end;
			int edgeIndex = 0;
			boost::property_map<graph_t, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
			boost::tie(start, end) = boost::edges(graph);
			for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
		}

		typedef std::vector<boost::graph_traits<DiscreteGermGrainSubObs::graph_t>::edges_size_type> bridges_storage_t;
		typedef boost::iterator_property_map<bridges_storage_t::iterator, boost::property_map<DiscreteGermGrainSubObs::graph_t, boost::edge_index_t>::type> bridges_map_t;
		
		bridges_storage_t ids(boost::num_edges(graph));
		bridges_map_t idsMap(ids.begin(), boost::get(boost::edge_index, graph));

		boost::bridges(graph, idsMap);
		std::size_t nIndependentComponents = *std::max_element(ids.begin(), ids.end())+1;*/

		//trivial case where there are no bridges
		//if(nIndependentComponents == 1)
		{
			for(int i = 1; i < std::numeric_limits<int>::max(); i++)
			{
				bool hasSingleComponent = false;
				outputObservation = getSingleComponentObservation(newRadius, randomSource, componentIDs, hasSingleComponent);
				if(hasSingleComponent) 
				{
					outputProbability = (float)(1.0/i);
					return;
				}
			}
			throw std::runtime_error("Numeric overflow in DiscreteGermGrainSubObs::getSingleComponentObservation");
			return;
		}

		//otherwise use the bridges to simplify estimation. 
		/*
		//turn these bridges into collections of points
		graph_t::edge_iterator start, end;
		boost::tie(start, end) = boost::edges(graph);
		std::vector<std::set<int> > componentPointSets(nIndependentComponents);
		bridges_storage_t::iterator idIterator = ids.begin();
		for(; start != end; start++, idIterator++)
		{
			std::set<int>& currentComponent = componentPointSets[*idIterator];
			currentComponent.insert(start->m_source);
			currentComponent.insert(start->m_target);
		}

		//get out the starting point of every bridge (There are two points, (x1, y1) and (x2, y2), and one will come first in lexographic ordering)

		//turn those sets of indices into vectors of points
		std::vector<std::vector<std::pair<Config::intType, Config::intType> > > componentPointVectors;
		for(int i = 0; i < nIndependentComponents; i++)
		{
			std::set<int>& componentPointSet = componentPointSets[i];
			std::vector<std::pair<Config::intType, Config::intType> > points;
			points.reserve(componentPointSet.size());
			for(std::set<int>::iterator pointIterator = componentPointSet.begin(); pointIterator != componentPointSet.end(); pointIterator++)
			{
				points.push_back(subPoints[*pointIterator]);
			}
			std::sort(points.begin(), points.end(), lexographicOrder);
			componentPointVectors.push_back(std::move(points));
		}
		//store copies of all the single component observations, for each 
		std::vector< std::vector<std::pair<Config::intType, Config::intType> > > singleComponentSections;
		for(int i = 0; i < nSimulations; i++)
		{
			//Simulate points
			std::vector<std::pair<Config::intType, Config::intType> > points;
			getRandomPointPattern(randomSource, points);
			
			//isSingleComponentWithRadius(const std::vector<std::pair<Config::intType, Config::intType> >& points, int radius, int gridDimension, std::vector<int>& vector);
		}
		outputObservations.resize(nIndependentComponents);
		*/
	}
	void DiscreteGermGrainSubObs::getRandomPointPattern(boost::mt19937& randomSource, vertexState* outputState)
	{
		boost::random::bernoulli_distribution<float> vertexDistribution(context.getOperationalProbabilityD());
		std::size_t nVertices = context.nVertices();
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
	}
	DiscreteGermGrainObs DiscreteGermGrainSubObs::getSingleComponentObservation(int newRadius, boost::mt19937& randomSource, std::vector<int>& componentIDs, bool& hasSingleComponent)
	{
		const std::size_t nVertices = context.nVertices();
		boost::shared_array<vertexState> newState(new vertexState[nVertices]);
		memcpy(newState.get(), state.get(), sizeof(vertexState)*nVertices);
		getRandomPointPattern(randomSource, newState.get());

		DiscreteGermGrainObs proposed(context, newState);
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		hasSingleComponent = discreteGermGrain::isSingleComponentPossible(context, proposed.getSubObservation(newRadius).getState(), componentIDs, stack);
		return proposed;
	}
	void DiscreteGermGrainSubObs::constructRadius1Graph(graph_t& graph, std::vector<int>& graphVertices) const
	{
		if(currentRadius != 1)
		{
			throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
		}
		graphVertices.clear();
		std::size_t nVertices = context.nVertices();
		const vertexState* stateRef = state.get();

		//graphVertices[i] is the vertex in the global graph that corresponds to vertex i in the returned graph
		//inverse is the inverse mapping, with 0's everywhere else. 
		int* inverse = (int*)alloca(nVertices * sizeof(int));
		//0 is used to identify a point which is not going to be contained within the returned graph
		memset(inverse, 0, sizeof(int)*nVertices);

		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(stateRef[i].state & (UNFIXED_MASK | ON_MASK))
			{
				graphVertices.push_back((int)i);
				inverse[i] = (int)graphVertices.size();
			}
		}

		graph = graph_t(graphVertices.size());
		const Context::inputGraph& originalGraph = context.getGraph();
		for(std::size_t i = 0; i < graphVertices.size(); i++)
		{
			Context::inputGraph::out_edge_iterator start, end;
			boost::tie(start, end) = boost::out_edges(graphVertices[i], originalGraph);
			while(start != end)
			{
				if(inverse[start->m_target] > 0 && (int)i < inverse[start->m_target] - 1)
				{
					boost::add_edge((int)i, inverse[start->m_target] - 1, graph);
				}
				start++;
			}
		}
	}
	boost::shared_array<const vertexState> DiscreteGermGrainSubObs::estimateRadius1(boost::mt19937& randomSource, int nSimulations, mpfr_class& outputProbability)
	{
		double openProbability = context.getOperationalProbabilityD();
		boost::bernoulli_distribution<float> bern(openProbability);
		if(currentRadius != 1)
		{
			throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
		}
		//Construct helper graph, containing everything except FIXED_OFF vertices
		graph_t graph;
		std::vector<int> graphVertices;
		constructRadius1Graph(graph, graphVertices);
		//assign edge indices
		{
			graph_t::edge_iterator start, end;
			int edgeIndex = 0;
			boost::property_map<graph_t, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
			boost::tie(start, end) = boost::edges(graph);
			for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
		}

		const vertexState* stateRef = state.get();
		std::size_t nVertices = context.nVertices();
		std::vector<singleComponent::graphType::vertex_descriptor> unfixedVertices;
		std::vector<singleComponent::graphType::vertex_descriptor> fixedVertices;
		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(stateRef[i].state & UNFIXED_MASK)
			{
				unfixedVertices.push_back(i);
			}
			if(stateRef[i].state &	FIXED_ON)
			{
				fixedVertices.push_back(i);
			}
		}
		//if everything is fixed at OFF, then we must have a connected graph
		if(fixedVertices.size() == 0 && unfixedVertices.size() == 0)
		{
			outputProbability = 1;
			return this->state;
		}
		if(boost::num_vertices(graph) == 1)
		{
			boost::shared_array<vertexState> newStates(new vertexState[1]);
			newStates[0] = vertexState::fixed_off();
			if(fixedVertices.size() == 1) outputProbability = 1;
			else outputProbability = openProbability;
			return newStates;
		}

		//get out biconnected components of helper graph
		typedef std::vector<boost::graph_traits<DiscreteGermGrainSubObs::graph_t>::edges_size_type> component_storage_t;
		typedef boost::iterator_property_map<component_storage_t::iterator, boost::property_map<graph_t, boost::edge_index_t>::type> component_map_t;
		
		component_storage_t biconnectedIds(boost::num_edges(graph));
		component_map_t componentMap(biconnectedIds.begin(), boost::get(boost::edge_index, graph));

		std::vector<int> articulationVertices;
		boost::biconnected_components(graph, componentMap, std::back_inserter(articulationVertices));

		//Count the number of biconnected components. I believe these are guaranteed to be contiguous....
		std::size_t nBiconnectedComponents = *std::max_element(biconnectedIds.begin(), biconnectedIds.end())+1;
#ifndef NDEBUG
		//but debug code to check, anyway
		std::vector<int> uniqueBiconnectedIds;
		uniqueBiconnectedIds.insert(uniqueBiconnectedIds.begin(), biconnectedIds.begin(), biconnectedIds.end());
		std::sort(uniqueBiconnectedIds.begin(), uniqueBiconnectedIds.end());
		uniqueBiconnectedIds.erase(std::unique(uniqueBiconnectedIds.begin(), uniqueBiconnectedIds.end()), uniqueBiconnectedIds.end());
		if(uniqueBiconnectedIds.size() != nBiconnectedComponents)
		{
			throw std::runtime_error("biconnected component ids were not contiguous");
		}
#endif
		int nNotAlreadyFixedArticulation = 0;
		//convert list of articulation vertices across to a bitset
		std::vector<bool> isArticulationVertex(nVertices, false);
		//Mark off each articulation point in the above vector, and count the number of extra points that we're fixing.
		for(std::vector<int>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
		{
			isArticulationVertex[graphVertices[*i]] = true;
			if(stateRef[graphVertices[*i]].state & UNFIXED_MASK) 
			{
				nNotAlreadyFixedArticulation++;
				fixedVertices.push_back(graphVertices[*i]);
			}
		}
		std::size_t nFixedVertices = fixedVertices.size();
		//allocate a set per biconnected component
		std::vector<std::set<int> > biConnectedSets(nBiconnectedComponents);
		//Add to each set the indices of the vertices that are contained in this biconnected component
		{
			graph_t::edge_iterator start, end;
			boost::property_map<graph_t, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
			boost::tie(start, end) = boost::edges(graph);
			for(; start != end; start++)
			{
				std::set<int>& currentComponentSet = biConnectedSets[biconnectedIds[boost::get(edgePropertyMap, *start)]];
				currentComponentSet.insert((int)graphVertices[start->m_source]);
				currentComponentSet.insert((int)graphVertices[start->m_target]);
			}
		}
		//vectors containing the fixed and unfixed vertices for each component
		std::vector<std::vector<singleComponent::graphType::vertex_descriptor> > fixedPointsPerComponent;
		std::vector<std::vector<singleComponent::graphType::vertex_descriptor> > unFixedPointsPerComponent;
		//we're also going to want to work out the number of fixed points per each component, because we're going to append on the unFixedPoints which end up being kept
		//to the end of the fixed points vector (just to save on memory allocations). 
		std::vector<int> nFixedPointsPerComponent;
		for(std::size_t componentCounter = 0; componentCounter < nBiconnectedComponents; componentCounter++)
		{
			std::set<int>& currentComponentVertices = biConnectedSets[componentCounter];
			std::vector<singleComponent::graphType::vertex_descriptor> currentComponentFixed, currentComponentUnFixed;
			for(std::set<int>::iterator vertexIterator = currentComponentVertices.begin(); vertexIterator != currentComponentVertices.end(); vertexIterator++)
			{
				if((stateRef[*vertexIterator].state & FIXED_MASK) || isArticulationVertex[*vertexIterator]) currentComponentFixed.push_back(*vertexIterator);
				else currentComponentUnFixed.push_back(*vertexIterator);
			}
			nFixedPointsPerComponent.push_back((int)currentComponentFixed.size());
			fixedPointsPerComponent.push_back(std::move(currentComponentFixed));
			unFixedPointsPerComponent.push_back(std::move(currentComponentUnFixed));
		}
		//vector containing counters. Each counter says the number of times that biconnected component j was found to be connected. 
		std::vector<int> connectedCounters(nBiconnectedComponents);
		//The vertices that will be retained in the final realisation
		std::vector<singleComponent::graphType::vertex_descriptor> finalVertices;
		//If we're doing unlimited simulation, then keep track of the number of components for which we haven't observed something connected. 
		//This is then used as the stopping condition instead of 
		std::size_t nZeroCounters = nBiconnectedComponents;
		//We need to take the same number of effective simulations, regardless of the number of biconnected components
		int nComponentRuns; 
		int nRemaining; 
		//But there are two modes of operation - Unlimited simulation or limited. 
		if(nSimulations == -1)
		{
			nComponentRuns = -1;
			nRemaining = 0;
		}
		else
		{
			nComponentRuns = (int)pow(nSimulations, 1.0/(float)nBiconnectedComponents);
			nRemaining = nSimulations - (int)(pow(nComponentRuns, nBiconnectedComponents) + 0.5);
		}
		int nSuccessfulRemaining = 0;
		//now generate random graphs
		{
			std::vector<int> scratchMemory;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			for(int i = 0; i < nComponentRuns; i++)
			{
				if(i < 0) throw std::runtime_error("Integer overflow!");
				//iterate over the biconnected components
				for(std::size_t j = 0; j < nBiconnectedComponents; j++)
				{
					std::vector<singleComponent::graphType::vertex_descriptor>& currentComponentFixed = fixedPointsPerComponent[j], currentComponentUnFixed = unFixedPointsPerComponent[j];
					currentComponentFixed.resize(nFixedPointsPerComponent[j]);
					for(std::vector<singleComponent::graphType::vertex_descriptor>::iterator k = currentComponentUnFixed.begin(); k != currentComponentUnFixed.end(); k++)
					{
						if(bern(randomSource)) currentComponentFixed.push_back(*k);
					}
					
					int nComponents = isSingleComponentSpecified(context, currentComponentFixed, scratchMemory, stack);
					if(nComponents == 1) 
					{
						connectedCounters[j]++;
						if(connectedCounters[j] == 1)
						{
							finalVertices.insert(finalVertices.end(), currentComponentFixed.begin(), currentComponentFixed.end());
							nZeroCounters--;
						}
					}
				}
				if(nComponentRuns == -1 && nZeroCounters == 0)
				{
					nComponentRuns = i+1;
					break;
				}
			}
			for(int i = 0; i < nRemaining; i++)
			{
				fixedVertices.resize(nFixedVertices);
				for(std::vector<singleComponent::graphType::vertex_descriptor>::iterator j = unfixedVertices.begin(); j != unfixedVertices.end(); j++) 
				{
					if(bern(randomSource))
					{
						fixedVertices.push_back(*j);
					}
				}
				int nComponents = isSingleComponentSpecified(context, fixedVertices, scratchMemory, stack);
				if(nComponents == 1) 
				{
					nSuccessfulRemaining++;
					if(nSuccessfulRemaining == 1) finalVertices = fixedVertices;
				}
			}
		}
		//only return any points at all if we found a situation where the whole thing is connected
		if(nZeroCounters == 0  || nSuccessfulRemaining > 0)
		{
			int productComponentSuccess = 1;
			for(std::size_t i = 0; i < nBiconnectedComponents; i++) productComponentSuccess *= connectedCounters[i];
			outputProbability = boost::multiprecision::pow(context.getOperationalProbability(), nNotAlreadyFixedArticulation) * ((float)(productComponentSuccess + nSuccessfulRemaining) / (float)nSimulations);
			
			boost::shared_array<vertexState> newStates(new vertexState[nVertices]);
			std::fill(newStates.get(), newStates.get()+nVertices, vertexState::fixed_off());
			for(std::vector<singleComponent::graphType::vertex_descriptor>::iterator i = finalVertices.begin(); i != finalVertices.end(); i++) newStates[*i].state = FIXED_ON;

			return newStates;
		}
		else 
		{
			outputProbability = 0;
			return boost::shared_array<vertexState>();
		}
	}
	/*__declspec(noinline) void DiscreteGermGrainSubObs::constructSubgraph(const std::set<int>& subgraphVertexIndices, const std::vector<std::pair<Config::intType, Config::intType> >& allPossiblePoints, graph_t& subgraph)
	{
		subgraph = graph_t(subgraphVertexIndices.size());
		int* vertexIndices = (int*)alloca(sizeof(int) * gridSize * gridSize);
		memset(vertexIndices, 0, sizeof(int) * gridSize * gridSize);
		int counter = 1;
		for(std::set<int>::const_iterator i = subgraphVertexIndices.begin(); i != subgraphVertexIndices.end(); i++)
		{
			std::pair<Config::intType, Config::intType> currentPoint = allPossiblePoints[*i];
			vertexIndices[currentPoint.first + currentPoint.second * gridSize] = counter++;
		}
		for(int i = 0; i < gridSize; i++)
		{
			for(int j = 0; j < gridSize; j++)
			{
				int currentPointIndex = vertexIndices[i + j * gridSize];
				if(currentPointIndex > 0)
				{
					int nextPointIndex = vertexIndices[i + 1 + j * gridSize];
					if(i < gridSize - 1 && nextPointIndex > 0)
					{
						boost::add_edge(currentPointIndex - 1, nextPointIndex - 1, subgraph);
					}
					nextPointIndex = vertexIndices[i + (j + 1) * gridSize];
					if(j < gridSize - 1 && nextPointIndex > 0)
					{
						boost::add_edge(currentPointIndex - 1, nextPointIndex - 1, subgraph);
					}
				}
			}
		}
	}*/
	DiscreteGermGrainSubObs::DiscreteGermGrainSubObs(DiscreteGermGrainSubObs&& other)
		:context(other.context)
	{
		std::swap(currentRadius, other.currentRadius);
		state.swap(other.state);
	}
	const vertexState* DiscreteGermGrainSubObs::getState() const
	{
		return state.get();
	}
	Context const& DiscreteGermGrainSubObs::getContext() const
	{
		return context;
	}
}

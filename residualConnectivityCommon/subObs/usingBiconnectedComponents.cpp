#include "subObs/usingBiconnectedComponents.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType& otherData)
			: ::discreteGermGrain::subObs::subObsWithRadius(context, state, radius)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
		}
		bool usingBiconnectedComponents::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		usingBiconnectedComponents::usingBiconnectedComponents(usingBiconnectedComponents&& other)
			: ::discreteGermGrain::subObs::subObsWithRadius(static_cast<::discreteGermGrain::subObs::subObsWithRadius&&>(other))
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void usingBiconnectedComponents::constructRadius1Graph(radiusOneGraphType& graph, std::vector<int>& graphVertices) const
		{
			if(radius != 1)
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

			graph = radiusOneGraphType(graphVertices.size());
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
		void usingBiconnectedComponents::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType&)const
		{
			boost::random::bernoulli_distribution<float> vertexDistribution(context.getOperationalProbabilityD());
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
		}
		boost::shared_array<const vertexState> usingBiconnectedComponents::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, mpfr_class& outputProbability) const
		{
			double openProbability = context.getOperationalProbabilityD();
			boost::bernoulli_distribution<float> bern(openProbability);
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			//Construct helper graph, containing everything except FIXED_OFF vertices
			radiusOneGraphType graph;
			std::vector<int> graphVertices;
			constructRadius1Graph(graph, graphVertices);
			//assign edge indices
			{
				radiusOneGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<radiusOneGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}

			const vertexState* stateRef = state.get();
			std::size_t nVertices = context.nVertices();
			std::vector<Context::inputGraph::vertex_descriptor> unfixedVertices;
			std::vector<Context::inputGraph::vertex_descriptor> fixedVertices;
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
			typedef std::vector<boost::graph_traits<radiusOneGraphType>::edges_size_type> component_storage_t;
			typedef boost::iterator_property_map<component_storage_t::iterator, boost::property_map<radiusOneGraphType, boost::edge_index_t>::type> component_map_t;

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
				radiusOneGraphType::edge_iterator start, end;
				boost::property_map<radiusOneGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++)
				{
					std::set<int>& currentComponentSet = biConnectedSets[biconnectedIds[boost::get(edgePropertyMap, *start)]];
					currentComponentSet.insert((int)graphVertices[start->m_source]);
					currentComponentSet.insert((int)graphVertices[start->m_target]);
				}
			}
			//vectors containing the fixed and unfixed vertices for each component
			std::vector<std::vector<Context::inputGraph::vertex_descriptor> > fixedPointsPerComponent;
			std::vector<std::vector<Context::inputGraph::vertex_descriptor> > unFixedPointsPerComponent;
			//we're also going to want to work out the number of fixed points per each component, because we're going to append on the unFixedPoints which end up being kept
			//to the end of the fixed points vector (just to save on memory allocations). 
			std::vector<int> nFixedPointsPerComponent;
			for(std::size_t componentCounter = 0; componentCounter < nBiconnectedComponents; componentCounter++)
			{
				std::set<int>& currentComponentVertices = biConnectedSets[componentCounter];
				std::vector<Context::inputGraph::vertex_descriptor> currentComponentFixed, currentComponentUnFixed;
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
			std::vector<Context::inputGraph::vertex_descriptor> finalVertices;
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
				for(int i = 0; i < nComponentRuns; i++)
				{
					if(i < 0) throw std::runtime_error("Integer overflow!");
					//iterate over the biconnected components
					for(std::size_t j = 0; j < nBiconnectedComponents; j++)
					{
						std::vector<Context::inputGraph::vertex_descriptor>& currentComponentFixed = fixedPointsPerComponent[j], currentComponentUnFixed = unFixedPointsPerComponent[j];
						currentComponentFixed.resize(nFixedPointsPerComponent[j]);
						for(std::vector<Context::inputGraph::vertex_descriptor>::iterator k = currentComponentUnFixed.begin(); k != currentComponentUnFixed.end(); k++)
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
					for(std::vector<Context::inputGraph::vertex_descriptor>::iterator j = unfixedVertices.begin(); j != unfixedVertices.end(); j++) 
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
				for(std::vector<Context::inputGraph::vertex_descriptor>::iterator i = finalVertices.begin(); i != finalVertices.end(); i++) newStates[*i].state = FIXED_ON;

				return newStates;
			}
			else 
			{
				outputProbability = 0;
				return boost::shared_array<vertexState>();
			}
		}
	}
}

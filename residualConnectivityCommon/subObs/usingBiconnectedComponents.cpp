#include "subObs/usingBiconnectedComponents.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "obs/usingBiconnectedComponents.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		usingBiconnectedComponents::usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType& otherData)
			: ::discreteGermGrain::subObs::subObsWithRadius(context, state, radius), weight(otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
		}
		bool usingBiconnectedComponents::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		usingBiconnectedComponents::usingBiconnectedComponents(usingBiconnectedComponents&& other)
			: ::discreteGermGrain::subObs::subObsWithRadius(static_cast<::discreteGermGrain::subObs::subObsWithRadius&&>(other)), weight(other.weight)
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
		void usingBiconnectedComponents::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
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
			otherData.weight = weight;
		}
		void usingBiconnectedComponents::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const
		{
			if(nSimulations <= 0)
			{
				throw std::runtime_error("Input nSimulations must be positive");
			}
			double openProbability = context.getOperationalProbabilityD();
			boost::bernoulli_distribution<float> bern(openProbability);
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			//Construct helper graph, containing everything except FIXED_OFF vertices. This is because in order to compute the biconnected components / articulation vertices, we need an actual graph object, not just a state vector
			radiusOneGraphType graph;
			std::vector<int> graphVertices;
			{
				constructRadius1Graph(graph, graphVertices);
				//assign edge indices
				radiusOneGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<radiusOneGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}
			// Determine which vertices are fixed on, and which are unfixed
			const vertexState* stateRef = state.get();
			std::size_t nVertices = context.nVertices();
			std::vector<Context::inputGraph::vertex_descriptor> unfixedVertices;
			std::vector<Context::inputGraph::vertex_descriptor> fixedOnVertices;
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
				if(potentiallyConnected)
				{
					::discreteGermGrain::obs::usingBiconnectedComponentsConstructorType weightType;
					weightType.weight = weight;
					for(int i = 0; i < nSimulations;i++)
					{
						outputObservations.push_back(observationType(context, this->state, weightType));
					}
				}
				return;
			}

			//get out biconnected components of helper graph (which has different vertex ids, remember)
			typedef std::vector<boost::graph_traits<radiusOneGraphType>::edges_size_type> component_storage_t;
			typedef boost::iterator_property_map<component_storage_t::iterator, boost::property_map<radiusOneGraphType, boost::edge_index_t>::type> component_map_t;
			component_storage_t biconnectedIds(boost::num_edges(graph));
			component_map_t componentMap(biconnectedIds.begin(), boost::get(boost::edge_index, graph));
			std::vector<int> articulationVertices;
			boost::biconnected_components(graph, componentMap, std::back_inserter(articulationVertices));

			//Count the number of biconnected components. I believe these are guaranteed to be contiguous....
			std::size_t nBiconnectedComponents = *std::max_element(biconnectedIds.begin(), biconnectedIds.end())+1;
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
					fixedOnVertices.push_back(graphVertices[*i]);
				}
			}
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
			mpfr_class newWeight = weight * boost::multiprecision::pow(context.getOperationalProbability(), nNotAlreadyFixedArticulation);
			observationConstructorType observationInput;
			//We need to take the same number of effective simulations, regardless of the number of biconnected components
			int nComponentRuns = (int)(pow(nSimulations, 1.0/(float)nBiconnectedComponents));
			int nRemaining = nSimulations - (int)(pow(nComponentRuns, nBiconnectedComponents) + 0.5);
			std::vector<observationType> generatedObservations;
			//connectedIndices[0] gives the indices of the entries in generatedObservations that are connected for biconnected component 0
			std::vector<std::vector<int> > connectedIndices(nBiconnectedComponents);
			for(int i = 0; i < nComponentRuns; i++)
			{
				boost::shared_array<vertexState> obsState(new vertexState[nVertices]);
				getObservation(obsState.get(), randomSource, observationInput);
				for(std::vector<int>::iterator j = articulationVertices.begin(); j != articulationVertices.end(); j++)
				{
					obsState[graphVertices[*j]].state = FIXED_ON;
				}
				observationInput.weight = newWeight;
				observationType obs(context, obsState, observationInput);
				for(std::size_t j = 0; j < nBiconnectedComponents; j++)
				{
					std::vector<Context::inputGraph::vertex_descriptor>& currentComponentFixed = fixedPointsPerComponent[j], currentComponentUnFixed = unFixedPointsPerComponent[j];
					if(partIsSingleComponent(context, obs.getState(), currentComponentFixed, currentComponentUnFixed, scratchMemory, stack)) connectedIndices[j].push_back(i);
				}
				generatedObservations.push_back(std::move(obs));
			}
			//Can we combine parts of the first nComponentRuns observations to get something connected?
			bool canCombine = true;
			for(std::vector<std::vector<int> >::const_iterator i = connectedIndices.begin(); i != connectedIndices.end(); i++)
			{
				if(i->size() == 0)
				{
					canCombine = false;
					break;
				}
			}
			if(canCombine)
			{
				//Now generate every possible observation that is connected, from the combinations drawn from the first nComponentRuns values in generatedObservations
				std::vector<std::vector<int>::const_iterator> currentPosition;
				for(std::size_t j = 0; j < nBiconnectedComponents; j++) currentPosition.push_back(connectedIndices[j].begin());
				while(true)
				{
					boost::shared_array<vertexState> newState(new vertexState[nVertices]);
					for(std::size_t j = 0; j < nBiconnectedComponents; j++)
					{
						int observationIndexThisComponent = *currentPosition[j];
						observationType& observationThisComponent = generatedObservations[observationIndexThisComponent];
						const vertexState* observationThisComponentState = observationThisComponent.getState();

						std::vector<Context::inputGraph::vertex_descriptor>& currentComponentFixed = fixedPointsPerComponent[j];
						std::vector<Context::inputGraph::vertex_descriptor>& currentComponentUnFixed = unFixedPointsPerComponent[j];
						for(std::vector<Context::inputGraph::vertex_descriptor>::iterator k = currentComponentFixed.begin(); k != currentComponentFixed.end(); k++)
						{
							newState[*k] = observationThisComponentState[*k];
						}
						for(std::vector<Context::inputGraph::vertex_descriptor>::iterator k = currentComponentUnFixed.begin(); k != currentComponentUnFixed.end(); k++)
						{
							newState[*k] = observationThisComponentState[*k];
						}
					}
					observationInput.weight = newWeight;
					outputObservations.push_back(observationType(context, newState, observationInput));
					std::size_t currentIncrementPoint = 0;
					while(currentIncrementPoint < nBiconnectedComponents)
					{
						currentPosition[currentIncrementPoint]++;
						if(currentPosition[currentIncrementPoint] == connectedIndices[currentIncrementPoint].end())
						{
							currentPosition[currentIncrementPoint] = connectedIndices[currentIncrementPoint].begin();
							currentIncrementPoint++;
						}
						else
						{
							break;
						}
					}
					if(currentIncrementPoint == nBiconnectedComponents)
					{
						break;
					}
				}
			}
			//Now for the remaining ones
			for(int i = nComponentRuns; i < nComponentRuns + nRemaining; i++)
			{
				boost::shared_array<vertexState> obsState(new vertexState[nVertices]);
				getObservation(obsState.get(), randomSource, observationInput);
				for(std::vector<int>::iterator j = articulationVertices.begin(); j != articulationVertices.end(); j++)
				{
					obsState[graphVertices[*j]].state = FIXED_ON;
				}
				observationInput.weight = newWeight;
				observationType obs(context, obsState, observationInput);
				if(isSingleComponentAllOn(context, obs.getState(), scratchMemory, stack)) 
				{
					outputObservations.push_back(std::move(obs));
				}
			}
		}
	}
}

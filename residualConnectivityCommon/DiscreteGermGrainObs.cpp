#include "DiscreteGermGrainObs.h"
#include "DiscreteGermGrainSubObs.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/connected_components.hpp>
#include <vector>
#include "lexographicOrder.h"
#include <boost/graph/copy.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
namespace discreteGermGrain
{
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::mt19937& randomSource)
		: context(context), state(NULL)
	{
		std::size_t nVertices = context.nVertices();

		boost::shared_array<vertexState> state(new vertexState[nVertices]);
		this->state = state;

		double openProbability = context.getOperationalProbabilityD();
		boost::random::bernoulli_distribution<float> vertexDistribution(openProbability);
		
		//Get out which vertices are present, and the count of the total number of present vertices
		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(vertexDistribution(randomSource))
			{
				state[i].state = UNFIXED_ON;
			}
			else state[i].state = UNFIXED_OFF;
		}
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> partialKnowledge, boost::mt19937& randomSource)
		: context(context)
	{
		std::size_t nVertices = boost::num_vertices(context.getGraph());
		boost::random::bernoulli_distribution<float> vertexDistribution(context.getOperationalProbabilityD());

		boost::shared_array<vertexState> state(new vertexState[nVertices]);
		for(std::size_t vertexCounter = 0; vertexCounter < nVertices; vertexCounter++)
		{
			if(partialKnowledge[vertexCounter].state & FIXED_MASK)
			{
				state[vertexCounter] = partialKnowledge[vertexCounter];
			}
			else if(vertexDistribution(randomSource))
			{
				state[vertexCounter].state = UNFIXED_ON;
			}
			else state[vertexCounter].state = UNFIXED_OFF;
		}

		this->state = state;
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(DiscreteGermGrainObs&& other)
		:context(other.context)
	{
		state.swap(other.state);
		subPointsCache.swap(other.subPointsCache);
	}
	DiscreteGermGrainSubObs DiscreteGermGrainObs::getSubObservation(int radius) const
	{
		cacheVectorType::const_iterator subPointsSearch;
		if((subPointsSearch = subPointsCache.find(radius)) != subPointsCache.end())
		{
			return DiscreteGermGrainSubObs(context, subPointsSearch->second, radius);
		}

		std::size_t nVertices = context.nVertices();
		const int* shortestDistances = context.getShortestDistances();

		int sourceVertex = 0;
		
		boost::shared_array<vertexState> newVertexStates(new vertexState[nVertices]);
		std::fill(newVertexStates.get(), newVertexStates.get()+nVertices, vertexState::fixed_off());
		vertexState* newVertexStatesRef = newVertexStates.get();

		while(sourceVertex < (int)nVertices)
		{
			//is this vertex marked as on, for one reason or another? If so continue from here
			if((state[sourceVertex].state & ON_MASK) > 0 && newVertexStatesRef[sourceVertex].state == FIXED_OFF)
			{
				newVertexStatesRef[sourceVertex].state = FIXED_ON;

				//Do we find another vertex in our search that is marked on, and is far enough away from the source?
				//If so retain it, it will be our new starting point. 
				//If no such found, we'll continue from finalSearchVertex+1
				bool found = false;
				std::size_t nextSourceVertex = -1;
				//keep copy of source vertex
				std::size_t copiedSourceVertex = sourceVertex;
				//we want to begin on the NEXT vertex
				sourceVertex++;
				while(sourceVertex <= (int)nVertices)
				{
					int previousState = state[sourceVertex].state;
					if(shortestDistances[copiedSourceVertex + nVertices * sourceVertex] <= radius)
					{
						if(previousState & FIXED_MASK) newVertexStatesRef[sourceVertex].state = previousState;
						else newVertexStatesRef[sourceVertex].state = UNFIXED_OFF;
					}
					else if(!found && (previousState & ON_MASK) > 0 && newVertexStatesRef[sourceVertex].state == FIXED_OFF)
					{
						nextSourceVertex = sourceVertex;
						found = true;
					}
					sourceVertex++;
				}
				//if we found another vertex, continue from there. If no, we're already at finalSearchVertex+1. 
				//Which is where we want to be.
				if(found)
				{
					sourceVertex = nextSourceVertex;
				}
			}
			else sourceVertex++;
		}
		subPointsCache.insert(std::make_pair(radius, newVertexStates));
		return DiscreteGermGrainSubObs(context, newVertexStates, radius);
	}
	Context const& DiscreteGermGrainObs::getContext() const
	{
		return context;
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> states)
		: context(context), state(states)
	{
	}
	DiscreteGermGrainObs& DiscreteGermGrainObs::operator=(DiscreteGermGrainObs&& other)
	{
		if(&context != &(other.context)) throw std::runtime_error("Attempting to mix different Contexts!");
		state.swap(other.state);
		subPointsCache.swap(other.subPointsCache);
		return *this;
	}
	const vertexState* DiscreteGermGrainObs::getState() const
	{
		return state.get();
	}
}

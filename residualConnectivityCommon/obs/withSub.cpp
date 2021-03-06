#include "obs/withSub.h"
namespace residualConnectivity
{
	void withSub::getSubObservation(int radius, vertexState* newState, const context& contextObj, const vertexState* oldStatesPtr)
	{
		std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
		const int* shortestDistances = contextObj.getShortestDistances();

		std::size_t sourceVertex = 0;

		std::fill(newState, newState+nVertices, vertexState::fixed_off());

		while(sourceVertex < nVertices)
		{
			//is this vertex marked as on, for one reason or another? If so continue from here
			if((oldStatesPtr[sourceVertex].state & ON_MASK) > 0 && newState[sourceVertex].state == FIXED_OFF)
			{
				newState[sourceVertex].state = FIXED_ON;

				//Do we find another vertex in our search that is marked on, and is far enough away from the source?
				//If so retain it, it will be our new starting point. 
				//If no such found, we'll continue from finalSearchVertex+1
				bool found = false;
				std::size_t nextSourceVertex = -1;
				//keep copy of source vertex
				std::size_t copiedSourceVertex = sourceVertex;
				//we want to begin on the NEXT vertex
				sourceVertex++;
				while(sourceVertex < nVertices)
				{
					int previousState = oldStatesPtr[sourceVertex].state;
					if(shortestDistances[copiedSourceVertex + nVertices * sourceVertex] <= radius)
					{
						if(previousState & FIXED_MASK) newState[sourceVertex].state = previousState;
						else newState[sourceVertex].state = UNFIXED_OFF;
					}
					else if(!found && (previousState & ON_MASK) > 0 && newState[sourceVertex].state == FIXED_OFF)
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
	}
	withSub::withSub(context const& contextObj)
		:observation(contextObj)
	{}
	withSub::withSub(context const& contextObj, boost::mt19937& randomSource)
		:observation(contextObj, randomSource)
	{}
	withSub::withSub(context const& contextObj, boost::shared_array<const vertexState> state)
		:observation(contextObj, state)
	{}
	withSub::withSub(withSub&& other)
		:observation(static_cast<observation&&>(other))
	{}
	withSub& withSub::operator=(withSub&& other)
	{
		this->observation::operator=(static_cast<observation&&>(other));
		return *this;
	}
	void withSub::getSubObservation(int radius, vertexState* outputState) const
	{
		getSubObservation(radius, outputState, contextObj, state.get());
	}
}

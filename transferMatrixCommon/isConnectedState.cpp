#include "isConnectedState.h"
#include "countSubgraphsCommon.h"
namespace discreteGermGrain
{
	inline bool isConnectedState(unsigned long long originalState, int gridDimension)
	{
		register unsigned long long state = originalState;
		//State is connected if it contains only one SINGLE group, or only one UPPER/MID*/LOWER set
		register VertexState previousState = (VertexState)(state & 7ULL);
		register bool foundUpper = previousState == UPPER;
		register bool foundSingle = previousState == SINGLE;
		state >>= 3;
		for(int i = 1; i < gridDimension; i++)
		{
			VertexState newState = (VertexState)(state & 7ULL);
			if(newState == SINGLE && previousState != SINGLE)
			{
				if(foundSingle || foundUpper) return false;
				foundSingle = true;
			}
			if(newState == UPPER && previousState != UPPER)
			{
				if(foundSingle || foundUpper) return false;
				foundUpper = true;
			}
			previousState = newState;
			state >>= 3;
		}
		return true;
		/* Above is the optimised version of:
		bool foundUpper = GETSTATE3(state, 0) == UPPER;
		bool foundSingle = GETSTATE3(state, 0) == SINGLE;
		VertexState previousState = (VertexState)GETSTATE3(state, 0);
		for(int i = 1; i < gridDimension; i++)
		{
			VertexState newState = (VertexState)GETSTATE3(state, i);
			if(newState == SINGLE && previousState != SINGLE)
			{
				if(foundSingle || foundUpper) return false;
				foundSingle = true;
			}
			if(newState == UPPER && previousState != UPPER)
			{
				if(foundSingle || foundUpper) return false;
				foundUpper = true;
			}
			previousState = newState;
		}
		return true;
		*/
	}
}
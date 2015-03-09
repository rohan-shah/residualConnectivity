#include "constructStates.h"
#include "countSubgraphsCommon.h"
#include "subgraphMacros.hpp"
namespace discreteGermGrain
{
	void constructStates(std::vector<unsigned long long>& states, std::vector<int>& stateVertexCounts, int gridDimension)
	{
		states.clear();
		states.push_back(0);
		stateVertexCounts.push_back(0);
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		unsigned long long currentState = 0;
		//0 - possible values are off, upper, single
		//1 - possible values are off, upper, single
		//2 - possible values are off, upper, mid, lower, single
		//3 - possible values are off, lower, single
		//4 - possible values are off, lower, single
		//counter is the next entry that needs to be incremented to something else (based on the possible values from the previous entry)
		int counter = gridDimension-1;
		int nLower = 0;
		int nUpper = 0;
#ifndef NDEBUG
		std::vector<VertexState> debugVector(gridDimension);
#endif
		while(true)
		{
			int currentCounterState = GETSTATE3(currentState, counter);
			int previousCounterState;
			if(counter > 0) previousCounterState = GETSTATE3(currentState, counter-1);
			else previousCounterState = OFF;
			//If it's not possible to increment the position pointer to by counter, then decrease the counter
			if(currentCounterState == SINGLE || (previousCounterState != OFF && currentCounterState != OFF))
			{
				if(counter == 0) break;
				counter--;
			}
			else if(previousCounterState != OFF && currentCounterState == OFF)
			{
				SETSTATE3(currentState, counter, previousCounterState);
				while(counter != gridDimension - 1) 
				{
					counter++;
					SETSTATE3(currentState, counter, OFF);
				}
			}
			//increment the position pointed to by counter (if possible).
			else
			{
				INCREMENTSTATE3(currentState, counter);
				if(currentCounterState + 1 == UPPER) nUpper++;
				else if(currentCounterState + 1 == LOWER) nLower++;
				if(currentCounterState == UPPER) nUpper--;
				else if(currentCounterState == LOWER)nLower--;

				int newCurrentState = currentCounterState + 1;
				//we update this state again in some cases (can't have mid without an open lower / upper, can't have
				//more lowers than uppers, etc.
				if((nLower > nUpper) || (newCurrentState == MID && nLower >= nUpper))
				{
					//keep counter the same. 
					continue;
				}
				while(counter != gridDimension - 1) 
				{
					counter++;
					SETSTATE3(currentState, counter, OFF);
				}
			}
			if(counter == gridDimension-1)
			{
				if(nUpper == nLower) 
				{
					//Here we work out the number of vertices in this state
					unsigned long long copiedCurrentState = currentState;
					int vertexCount = 0;
					while(copiedCurrentState)
					{
						if(copiedCurrentState & 7ULL) vertexCount++;
						copiedCurrentState >>= 3;
					}
					states.push_back(currentState);
					stateVertexCounts.push_back(vertexCount);
				}
			}
		}
		//add in -1 as the last state which corresponds to an empty column AFTER there's already been something
		//from this state we can only stay in this state. Whereas from the normal empty state we can have points occurr...
		states.push_back((unsigned long long)-1LL);
		stateVertexCounts.push_back(0);
#ifndef NDEBUG
		for(std::vector<unsigned long long>::iterator i = states.begin(); i != states.end(); i++)
		{
			for(int j = 0; j < gridDimension; j++)
			{
				debugVector[j] = (VertexState)GETSTATE3(*i, j);
			}
		}
#endif
	}
}
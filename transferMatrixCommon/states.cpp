#include "transferMatrixCommon/states.h"
#include "transferMatrixCommon/countSubgraphsCommon.h"
#include "transferMatrixCommon/subgraphMacros.hpp"
#include <boost/iterator/counting_iterator.hpp>
namespace residualConnectivity
{
	transferStates::transferStates(int gridDimension)
		:gridDimension(gridDimension)
	{
		//Put in the zero state - This corresponds to an empty row / column, BEFORE any rows/columns with active vertices
		allStates.push_back(0);
		stateVertexCounts.push_back(0);

		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		//This is the current state, as a list of three-bit values, with values given by VertexState. Note that if a two consecutive values are both not OFF, then they must in fact have the same value. 
		unsigned long long currentState = 0;
		//counter is the index of the three-bit entry that needs to be incremented to something else (based on the possible values from the previous entry)
		int counter = gridDimension-1;
		//Number of upper and lower values so far
		int nLower = 0;
		int nUpper = 0;
		while(true)
		{
			int currentCounterState = GETSTATE3(currentState, counter);
			int previousCounterState;
			if(counter > 0) previousCounterState = GETSTATE3(currentState, counter-1);
			else previousCounterState = OFF;
			//If it's not possible to increment the position pointed to by counter, then decrease the counter
			if(currentCounterState == SINGLE || (previousCounterState != OFF && currentCounterState != OFF))
			{
				if(counter == 0) break;
				counter--;
			}
			//If we have a value for the previous entry (which is not OFF) then set the value at the current position to the same as the previous entry, and set all the following ones to OFF
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
				int newCurrentState = currentCounterState + 1;
				if(newCurrentState == UPPER) nUpper++;
				else if(newCurrentState == LOWER) nLower++;
				if(currentCounterState == UPPER) nUpper--;
				else if(currentCounterState == LOWER)nLower--;

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
						//if the three-bit value is not 0, then there's a vertex there
						if(copiedCurrentState & 7ULL) vertexCount++;
						copiedCurrentState >>= 3;
					}
					allStates.push_back(currentState);
					stateVertexCounts.push_back(vertexCount);
				}
			}
		}
		//add in -1 as the last state which corresponds to an empty column AFTER there's already been something
		//from this state we can only stay in this state. Whereas from the normal empty state we can have points occur.
		allStates.push_back((unsigned long long)-1LL);
		stateVertexCounts.push_back(0);
	}
	const std::vector<unsigned long long>& transferStates::getStates() const
	{
		return allStates;
	}
	const std::vector<int>& transferStates::getStateVertexCounts() const
	{
		return stateVertexCounts;
	}
	int transferStates::getGridDimension() const
	{
		return gridDimension;
	}
	struct indicesByState
	{
	private:
		const std::vector<unsigned long long>& states;
	public:
		indicesByState(const std::vector<unsigned long long>& states)
			:states(states)
		{}
		bool operator()(int i, int j)
		{
			return states[i] < states[j];
		}
	};
	void transferStates::sort()
	{
		const std::size_t stateSize = allStates.size();
		std::vector<int> indices(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)stateSize));

		indicesByState indicesByStateSorter(allStates);
		std::sort(indices.begin() + 1, indices.end()-1, indicesByStateSorter);
		std::vector<int> newStateVertexCounts(stateSize);
		for(std::size_t i = 0; i < stateSize; i++)
		{
			newStateVertexCounts[i] = stateVertexCounts[indices[i]];
		}
		newStateVertexCounts.swap(stateVertexCounts);
		//Now we sort the states, except the first and last values
		std::sort(allStates.begin()+1, allStates.end()-1);
	
	}
}

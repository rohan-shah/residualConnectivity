#include "countSubgraphsCommon.h"
#include "constructStates.h"
#include <boost/scoped_array.hpp>
#include "subgraphMacros.hpp"
namespace discreteGermGrain
{
	void generateCachedIndices(unsigned long long binaryEncoding, int gridDimension, std::vector<std::size_t>& cachedIndices, int nGroups, const std::vector<unsigned long long>& states)
	{
		const std::size_t cachedIndicesSize = ((std::size_t)1) << (2*nGroups);
		cachedIndices.resize(cachedIndicesSize);
#ifndef NDEBUG
		std::fill(cachedIndices.begin(), cachedIndices.end(), (std::size_t)0);
#endif
		//set up cache vector
		for(std::size_t cacheIndex = 0; cacheIndex < cachedIndicesSize; cacheIndex++)
		{
			//The cache index represents nGroups different sets of 2-bit values, we have to expand this out
			register unsigned long long expanded = 0;
			register int cacheIndexCounter = 0;
			//Going to alter these two, so make copies
			register unsigned long long copiedBinaryEncoding = binaryEncoding;
			register std::size_t copiedCacheIndex = cacheIndex;
			for(int i = 0; i < gridDimension; i++)
			{
				if(copiedBinaryEncoding & 1ULL)
				{
					unsigned long long cacheIndexState = (copiedCacheIndex & 3ULL)+1;
					expanded |= (cacheIndexState << (3*i));
					if(i < gridDimension - 1 && !(copiedBinaryEncoding & 2ULL)) 
					{
						copiedCacheIndex >>= 2;
						cacheIndexCounter++;
					}
				}
				copiedBinaryEncoding >>= 1;
			}
			std::vector<unsigned long long>::const_iterator searchForExpanded;
			if(*(searchForExpanded = std::lower_bound(states.begin(), states.end(), expanded)) == expanded)
			{
				cachedIndices[cacheIndex] = std::distance(states.begin(), searchForExpanded);
			}
		}
		//The above is an optimised version of:
		/*
		for(std::size_t cacheIndex = 0; cacheIndex < cachedIndicesSize; cacheIndex++)
		{
			//The cache index represents nGroups different sets of 2-bit values, we have to expand this out
			unsigned long long expanded = 0;
			int cacheIndexCounter = 0;
			for(int i = 0; i < gridDimension; i++)
			{
				if(binaryEncoding & (1ULL << i))
				{
					VertexState cacheIndexState = (VertexState)GETSTATE2(cacheIndex, cacheIndexCounter);
					SETSTATE3(expanded, i, cacheIndexState);
					if(i < gridDimension - 1 && !(binaryEncoding & (1ULL << (i+1)))) cacheIndexCounter++;
				}
			}
			std::vector<unsigned long long>::const_iterator searchForExpanded;
			if((searchForExpanded = std::find(states.begin(), states.end(), expanded)) != states.end())
			{
				cachedIndices[cacheIndex] = std::distance(states.begin(), searchForExpanded);
			}
		}
		*/
	}
	void generateBinaryMasks(const std::vector<unsigned long long>& states, std::vector<unsigned long long>& binaryMasks, int gridDimension)
	{
		const std::size_t stateSize = states.size();
		for(std::size_t stateCounter = 0; stateCounter < stateSize-1; stateCounter++)
		{
			register unsigned long long state = states[stateCounter];
			register unsigned long long binaryMask = 0;
			register unsigned long long bit = 1;
			for(int i = 0; i < gridDimension; i++)
			{
				if(state & 7)
				{
					binaryMask |= bit;
				}
				bit <<= 1;
				state >>= 3;
			}
			unsigned long long inMemoryBinaryMask = binaryMask;
			binaryMasks.push_back(inMemoryBinaryMask);
			//The above is an optimised version of:
			/*for(int i = 0; i < gridDimension; i++)
			{
				if(GETSTATE3(state, i)) binaryMask += (1ULL << i);
			}
			binaryMasks.push_back(binaryMask);
			*/
		}
	}
	std::string stateToString(unsigned long long state, int gridDimension)
	{
		if(state == (unsigned long long)-1) return "-1";
		std::string returnValue = "";
		switch(GETSTATE3(state, 0))
		{
		case OFF:
			returnValue += "-";
			break;
		case UPPER:
			returnValue += "U";
			break;
		case MID:
			returnValue += "M";
			break;
		case LOWER:
			returnValue += "L";
			break;
		case SINGLE:
			returnValue += "S";
			break;
		}
		for(int i = 1; i < gridDimension; i++)
		{
			returnValue += ",";
			switch(GETSTATE3(state, i))
			{
			case OFF:
				returnValue += "-";
				break;
			case UPPER:
				returnValue += "U";
				break;
			case MID:
				returnValue += "M";
				break;
			case LOWER:
				returnValue += "L";
				break;
			case SINGLE:
				returnValue += "S";
				break;
			}
		}
		return returnValue;
	}
	void generateStateGroupBinaryMasks(const std::vector<unsigned long long>& states, std::vector<std::vector<unsigned long long> >& stateGroupBinaryMasks, int gridDimension)
	{
		const std::size_t stateSize = states.size();
		int* stack = new int[gridDimension];
		//no mask for the -1 state
		for(std::size_t i = 0; i < stateSize-1; i++)
		{
			int stackOffset = 0;
			register unsigned long long state = states[i];
			register unsigned long long bit = 1ULL;
			std::vector<unsigned long long> masks;
			register int nextFreeID = 0;
			register int currentGroup = -1;
			for(int i = 0; i < gridDimension; i++)
			{
				int currentState3 = (int)(state & 7ULL);
				if(currentState3 == UPPER)
				{
					masks.push_back(0);
					do
					{
						masks[nextFreeID] |= bit;
						bit <<= 1;
						state >>= 3;
						i++;
					}
					while(state & 7ULL);
					stack[stackOffset++] = currentGroup;
					currentGroup = nextFreeID;
					nextFreeID++;
				}
				else if(currentState3 == LOWER)
				{
					do
					{
						masks[currentGroup] |= bit;
						bit <<= 1;
						state >>= 3;
						i++;
					}
					while(state & 7ULL);
					currentGroup = stack[--stackOffset];
				}
				else if(currentState3 == MID)
				{
					do
					{
						masks[currentGroup] |= bit;
						bit <<= 1;
						state >>= 3;
						i++;
					}
					while(state & 7ULL);
				}
				else if(currentState3 == SINGLE)
				{
					masks.push_back(0);
					do
					{
						masks[nextFreeID] |= bit;
						bit <<= 1;
						state >>= 3;
						i++;
					}
					while(state & 7ULL);
					nextFreeID++;
				}
				bit <<= 1;
				state >>= 3;
			}
			stateGroupBinaryMasks.push_back(std::move(masks));
#ifndef NDEBUG
			int stackOffsetCopy = stackOffset;
			assert(stackOffsetCopy == 0);
#endif
		}
		delete[] stack;
		/* Above is the optimised version of:
		for(int i = 0; i < stateSize-1; i++)
		{
			unsigned long long state = states[i];
			std::vector<unsigned long long> masks;
			int previousState3 = OFF;
			int nextFreeID = 0;
			int currentGroup = -1;
			for(int i = 0; i < gridDimension; i++)
			{
				int currentState3 = GETSTATE3(state, i);
				if(currentState3 == UPPER)
				{
					masks.push_back(0);
					do
					{
						masks[nextFreeID] |= (1ULL << i);
						i++;
					}
					while(GETSTATE3(state, i) != OFF);
					stack.push_back(currentGroup);
					currentGroup = nextFreeID;
					nextFreeID++;
				}
				else if(currentState3 == LOWER)
				{
					do
					{
						masks[currentGroup] |= (1ULL << i);
						i++;
					}
					while(GETSTATE3(state, i) != OFF);
					currentGroup = *stack.rbegin();
					stack.pop_back();
				}
				else if(currentState3 == MID)
				{
					do
					{
						masks[currentGroup] |= (1ULL << i);
						i++;
					}
					while(GETSTATE3(state, i) != OFF);
				}
				else if(currentState3 == SINGLE)
				{
					masks.push_back(0);
					do
					{
						masks[nextFreeID] |= (1ULL << i);
						i++;
					}
					while(GETSTATE3(state, i) != OFF);
					nextFreeID++;
				}
			}
			stateGroupBinaryMasks.push_back(std::move(masks));
#ifndef NDEBUG
			assert(stack.size() == 0);
#endif
		}
		*/
	}
}

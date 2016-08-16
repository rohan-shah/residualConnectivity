#include <iostream>
#include "context.h"
#include "constructGraphs.h"
#include <boost/scoped_array.hpp>
#include "isSingleComponentWithRadius.h"
#include <omp.h>
namespace residualConnectivity
{
	inline int countSetBits(std::size_t i)
	{
		i = i - ((i >> 1) & 0x5555555555555555ULL);
		i = (i & 0x3333333333333333ULL) + ((i >> 2) & 0x3333333333333333ULL);
		return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FULL) * 0x101010101010101ULL) >> 56);
	}
	mpz_class gridCountSpecificSizeMultiThreaded(int gridDimension, int size)
	{
		context::inputGraph graph;
		std::vector<context::vertexPosition> vertexPositions;
		constructGraphs::squareGrid(gridDimension, graph, vertexPositions);

		std::size_t connected = 0;
		std::size_t maxValue = (1ULL << (gridDimension*gridDimension));
		std::size_t increment = (1ULL << 32);
		std::size_t start = 0;
		#pragma omp parallel firstprivate(start)
		{
			std::vector<int> connectedComponents;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
			std::vector<vertexState> state;
			state.resize(gridDimension*gridDimension);
			for(; start < maxValue; start += increment)
			{
				std::size_t end = std::min(start + increment, maxValue);
				#pragma omp for schedule(static, 1) 
				for(std::ptrdiff_t counter = (std::ptrdiff_t)start; counter < (std::ptrdiff_t)end; counter++)
				{
					if(countSetBits((std::size_t)counter) == size)
					{
						for(int i = 0; i < gridDimension*gridDimension; i++)
						{
							if(((std::size_t)counter) & (1ULL << i)) state[i].state = FIXED_ON;
							else state[i].state = FIXED_OFF;
						}
						if(isSingleComponentPossible(graph, &(state[0]), connectedComponents, stack))
						{
							#pragma omp critical
							connected++;
						}
					}
				}
			}
		}
		return connected;
	}
	mpz_class gridCountSpecificSizeSingleThreaded(int gridDimension, int size)
	{
		context::inputGraph graph;
		std::vector<context::vertexPosition> vertexPositions;
		constructGraphs::squareGrid(gridDimension, graph, vertexPositions);
		std::size_t connected = 0;
		std::size_t maxValue = (1ULL << gridDimension*gridDimension);
		{
			std::vector<int> connectedComponents;
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
			std::vector<vertexState> state;
			state.resize(gridDimension*gridDimension);
			for(std::size_t counter = 0; counter < maxValue; counter++)
			{
				if(countSetBits(counter) == size)
				{
					for(int i = 0; i < gridDimension*gridDimension; i++)
					{
						if(counter & (1ULL << i)) state[i].state = FIXED_ON;
						else state[i].state = FIXED_OFF;
					}
					if(isSingleComponentPossible(graph, &(state[0]), connectedComponents, stack))
					{
						connected++;
					}
				}
			}
		}
		return connected;
	}
}


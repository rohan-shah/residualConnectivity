#include "countSubgraphsBySize.h"
#include "countSubgraphsCommon.h"
#include "constructMatrices.h"
#include "constructStates.h"
#include "sortStatesAndVertexCounts.h"
namespace discreteGermGrain
{
	void countSubgraphsBySizeMultiThreaded(mpz_class* counts, int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		return countSubgraphsBySizeSingleThreaded(counts, gridDimension, transitionMatrix, nonZeroCount);
	}
	void countSubgraphsBySizeSingleThreaded(mpz_class* counts, int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		std::vector<unsigned long long> states;
		std::vector<int> stateVertexCounts;
		constructStates(states, stateVertexCounts, gridDimension);
		
		const std::size_t stateSize = states.size();

		sortStatesAndVertexCounts(states, stateVertexCounts);


		FinalColumnVector final;
		InitialRowVector initial;
		constructMatricesDense(transitionMatrix, final, initial, gridDimension, states, nonZeroCount);

		LargeDenseIntMatrix product;
		product.setIdentity(stateSize, stateSize);

		//set up count vectors
		std::vector<InitialRowVector> countsBySizes;
		countsBySizes.resize(gridDimension*gridDimension+1);
		for(int i = 0; i < gridDimension*gridDimension+1; i++)
		{
			countsBySizes[i].resize(stateSize);
			countsBySizes[i].fill(0L);
		}

		//set up the initial total numbers of vertices
		for(std::size_t i = 0; i < stateSize; i++)
		{
			if(initial(i) > 0) countsBySizes[stateVertexCounts[i]][i] = 1L;
		}

		for(int i = 0; i < gridDimension - 1; i++)
		{
			//We can only reach [gridDimension*gridDimension -gridDimension+1, gridDimension*gridDimension] in the last step, so we don't need to consider these
			for(int j = gridDimension*gridDimension-gridDimension; j >= 0; j--)
			{
				InitialRowVector tmp = countsBySizes[j] * transitionMatrix;
				mpz_class previousTerminated = countsBySizes[j][stateSize-1];
				for(std::size_t k = 0; k < stateSize; k++)
				{
					countsBySizes[j + stateVertexCounts[k]][k] += tmp[k];
				}
				mpz_class newTerminated = countsBySizes[j][stateSize-1];
				countsBySizes[j].fill(0L);
				countsBySizes[j][stateSize-1] = newTerminated - previousTerminated;
			}
			countsBySizes[0][0] = 1;
		}

		for(int i = 0; i < gridDimension*gridDimension+1; i++)
		{
			counts[i] = countsBySizes[i].dot(final);
		}
	}
}

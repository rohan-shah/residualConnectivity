#include "transferMatrixCommon/countSubgraphsBySize.h"
#include "transferMatrixCommon/countSubgraphsCommon.h"
#include "transferMatrixCommon/constructMatrices.h"
namespace residualConnectivity
{
#ifdef USE_OPENMP
	void countSubgraphsBySizeMultiThreaded(mpz_class* counts, int gridDimension, TransitionMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsBySizeLogger* logger)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		logger->beginComputeStates();
			transferStates states(gridDimension);
			states.sort();
		logger->endComputeStates(states);

		const std::vector<unsigned long long>& allStates = states.getStates();
		const std::vector<int>& stateVertexCounts = states.getStateVertexCounts();
		const std::size_t stateSize = allStates.size();

		FinalColumnVector final;
		InitialRowVector initial;
		logger->beginConstructDenseMatrices();
			constructMatricesDense(transitionMatrix, final, initial, states, nonZeroCount);
		logger->endConstructDenseMatrices(transitionMatrix, final, initial, nonZeroCount);

		LargeDenseIntMatrix product;
		product.setIdentity(stateSize, stateSize);

		//set up count vectors
		std::vector<InitialRowVector> countsBySizes;
		//We need a copy if we're going to multi-thread
		std::vector<InitialRowVector> countsBySizesCopy;
		countsBySizes.resize(gridDimension*gridDimension+1);
		countsBySizesCopy.resize(gridDimension*gridDimension+1);
		for(int i = 0; i < gridDimension*gridDimension+1; i++)
		{
			countsBySizes[i].resize(stateSize);
			countsBySizes[i].fill(0L);
			countsBySizesCopy[i].resize(stateSize);
			countsBySizesCopy[i].fill(0L);
		}

		//set up the initial total numbers of vertices
		for(std::size_t i = 0; i < stateSize; i++)
		{
			if(initial(i) > 0) countsBySizes[stateVertexCounts[i]][i] = 1L;
		}

		logger->beginMultiplications();
		#pragma omp parallel
		{
			for(int i = 0; i < gridDimension - 1; i++)
			{
				#pragma omp barrier
				if(omp_get_thread_num() == 0)
				{
					for(int i = 0; i < gridDimension*gridDimension+1; i++) countsBySizesCopy[i].fill(0L);
				}
				//We can only reach [gridDimension*gridDimension -gridDimension+1, gridDimension*gridDimension] in the last step, so we don't need to consider these
				#pragma omp for
				for(int j = gridDimension*gridDimension-gridDimension; j >= 0; j--)
				{
					InitialRowVector tmp = countsBySizes[j] * transitionMatrix;
					#pragma omp critical
					{
						for(std::size_t k = 0; k < stateSize; k++)
						{
							countsBySizesCopy[j + stateVertexCounts[k]][k] += tmp[k];
						}
					}
				}
				#pragma omp barrier
				if(omp_get_thread_num() == 0) 
				{
					logger->completedMultiplicationStep(i+1, gridDimension);
					countsBySizesCopy.swap(countsBySizes);
				}
			}
		}
		logger->endMultiplications();

		logger->beginCheckFinalStates();
		for(int i = 0; i < gridDimension*gridDimension+1; i++)
		{
			counts[i] = countsBySizes[i].dot(final);
		}
		logger->endCheckFinalStates();
	}
#endif
	void countSubgraphsBySizeSingleThreaded(mpz_class* counts, int gridDimension, TransitionMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsBySizeLogger* logger)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		logger->beginComputeStates();
			transferStates states(gridDimension);
			states.sort();
		logger->endComputeStates(states);

		const std::vector<unsigned long long>& allStates = states.getStates();
		const std::vector<int>& stateVertexCounts = states.getStateVertexCounts();
		const std::size_t stateSize = allStates.size();

		FinalColumnVector final;
		InitialRowVector initial;
		logger->beginConstructDenseMatrices();
			constructMatricesDense(transitionMatrix, final, initial, states, nonZeroCount);
		logger->endConstructDenseMatrices(transitionMatrix, final, initial, nonZeroCount);

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

		logger->beginMultiplications();
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
			logger->completedMultiplicationStep(i+1, gridDimension);
		}
		logger->endMultiplications();

		logger->beginCheckFinalStates();
		for(int i = 0; i < gridDimension*gridDimension+1; i++)
		{
			counts[i] = countsBySizes[i].dot(final);
		}
		logger->endCheckFinalStates();
	}
}

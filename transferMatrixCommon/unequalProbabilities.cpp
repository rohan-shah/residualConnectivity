#include "transferMatrixCommon/unequalProbabilities.h"
namespace residualConnectivity
{
	void transferMatrixUnequalProbabilities(mpfr_class& result, int gridDimension, LargeSparseIntMatrix& transitionMatrix, transferMatrixUnequalProbabilitiesLogger* logger, LargeDenseFloatMatrix& probabilities)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		if(probabilities.rows() != gridDimension || probabilities.cols() != gridDimension)
		{
			throw std::runtime_error("Input probabilities had the wrong dimension");
		}
		LargeDenseFloatMatrix complementaryProbabilities(gridDimension, gridDimension);
		for(int row = 0; row < gridDimension; row++)
		{
			for(int column = 0; column < gridDimension; column++)
			{
				complementaryProbabilities(row, column) = 1 - probabilities(row, column);
			}
		}

		logger->beginComputeStates();
			transferStates states(gridDimension);
			states.sort();
		logger->endComputeStates(states);

		const std::vector<unsigned long long>& allStates = states.getStates();
		const std::size_t stateSize = allStates.size();

		FinalColumnVector final;
		InitialRowVector initial;
		std::size_t nonZeroCount = 0;
		logger->beginConstructSparseMatrices();
			constructMatricesSparse(transitionMatrix, final, initial, states, nonZeroCount);
		logger->endConstructSparseMatrices(transitionMatrix, final, initial, nonZeroCount);

		//set up count vectors
		std::vector<mpfr_class> stateProbabilities(allStates.size(), 0);
		//set up the initial probabilities
#ifdef USE_OPENMP
		#pragma omp parallel for
#endif
		for(std::size_t i = 0; i < stateSize; i++)
		{
			if(initial(i) > 0) 
			{
				mpfr_class product = 1;
				unsigned long long encodedState = allStates[i];
				for(int j = 0; j < gridDimension; j++)
				{
					if(encodedState & (7ULL << 3*j))
					{
						product *= probabilities(j, 0);
					}
					else product *= complementaryProbabilities(j, 0);
				}
				stateProbabilities[i] = product;
			}
		}

		logger->beginMultiplications();
#ifdef USE_OPENMP
		#pragma omp parallel
#endif
		{
			std::vector<mpfr_class> privateCopy(stateSize);
			for(int i = 1; i < gridDimension; i++)
			{
				std::fill(privateCopy.begin(), privateCopy.end(), 0);
				//We can only reach [gridDimension*gridDimension -gridDimension+1, gridDimension*gridDimension] in the last step, so we don't need to consider these
#ifdef USE_OPENMP
				#pragma omp barrier
				#pragma omp for
#endif
				for(LargeSparseIntMatrix::iterator j = transitionMatrix.begin(); j < transitionMatrix.end(); j++)
				{
					unsigned long long sourceIndex = std::distance(transitionMatrix.begin(), j);
					for(std::vector<unsigned long long>::iterator k = j->begin(); k != j->end(); k++)
					{
						unsigned long long destinationIndex = *k;
						unsigned long long destinationState = allStates[destinationIndex];
						mpfr_class product = 1;
						if(destinationIndex == stateSize-1)
						{
							for(int l = 0; l < gridDimension; l++)
							{
								product *= complementaryProbabilities(l, i);
							}
						}
						else
						{
							for(int l = 0; l < gridDimension; l++)
							{
								if(destinationState & (7ULL << 3*l)) product *= probabilities(l, i);
								else product *= complementaryProbabilities(l, i);
							}
						}
						privateCopy[destinationIndex] += stateProbabilities[sourceIndex] * product;
					}
				}
#ifdef USE_OPENMP
				#pragma omp barrier
				if(omp_get_thread_num() == 0)
#endif
				{
					std::fill(stateProbabilities.begin(), stateProbabilities.end(), 0);
				}
#ifdef USE_OPENMP
				#pragma omp barrier
				#pragma omp critical
#endif
				{
					for(std::size_t j = 0; j < stateSize; j++)
					{
						stateProbabilities[j] += privateCopy[j];
					}
				}
			}
		}
		logger->endMultiplications();

		result = 0;
		logger->beginCheckFinalStates();
		for(std::size_t i = 0; i < stateSize; i++)
		{
			if(final(i))
			{
				result += stateProbabilities[i];
			}
		}
		logger->endCheckFinalStates();
	}
}

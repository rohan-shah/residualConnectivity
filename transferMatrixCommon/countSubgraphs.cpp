#include "countSubgraphsCommon.h"
#include "states.h"
#include "countSubgraphs.h"
#include <stdexcept>
#include <vector>
namespace residualConnectivity
{
	mpz_class countSubgraphsSingleThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsLogger* logger)
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
		const std::size_t stateSize = allStates.size();

		FinalColumnVector final;
		InitialRowVector initial;
		logger->beginConstructDenseMatrices();
			constructMatricesDense(transitionMatrix, final, initial, states, nonZeroCount);
		logger->endConstructDenseMatrices(transitionMatrix, final, initial, nonZeroCount);

		/*Eigen version
		LargeIntMatrix products(transitionMatrix);
		for(int i = 0; i < gridDimension-2; i++)
		{
			products *= transitionMatrix;
		}
		return initial * products * final;
		*/


		//complicated EIGEN version
		//work out how many multiplications we need to do in the while loop
		int finalPower = gridDimension - 1;
		int nSteps = 0;
		int currentResultPower = 0;
		if(finalPower & 1) currentResultPower = 1;
		int currentSquarePower = 1;
		while(currentResultPower != finalPower)
		{
			nSteps++;
			currentSquarePower <<= 1;
			if(finalPower & currentSquarePower)
			{
				currentResultPower += currentSquarePower;
				nSteps++;
			}
		}

		if(finalPower == 1)
		{
			return initial * transitionMatrix * final;
		}
		else
		{
			currentResultPower = 0;
			LargeDenseIntMatrix result;
			if(finalPower & 1)
			{
				result = transitionMatrix;
				currentResultPower = 1;
			}
			else
			{
				result.setIdentity(stateSize, stateSize);
			}
			currentSquarePower = 1;
			LargeDenseIntMatrix squared(transitionMatrix);
			
			logger->beginMultiplications();
			int currentStep = 0;
			while(currentResultPower != finalPower)
			{
				squared *= squared;
				logger->completedMultiplicationStep(++currentStep, nSteps);
				currentSquarePower <<= 1;
				if(finalPower & currentSquarePower)
				{
					result *= squared;
					logger->completedMultiplicationStep(++currentStep, nSteps);
					currentResultPower += currentSquarePower;
				}
			}
			logger->endMultiplications();
			return initial * result * final;
		}
	}
	mpz_class countSubgraphsMultiThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsLogger* logger)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		return countSubgraphsSingleThreaded(gridDimension, transitionMatrix, nonZeroCount, logger);
	}
}

#include "countSubgraphsCommon.h"
#include "constructMatrices.h"
#include "constructStates.h"
#include "countSubgraphs.h"
#include <stdexcept>
#include <vector>
namespace discreteGermGrain
{
	mpz_class countSubgraphsSingleThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}

		std::vector<unsigned long long> states;
		{
			std::vector<int> stateVertexCounts;
			constructStates(states, stateVertexCounts, gridDimension);
		}

		//We don't need the state vertex counts here, so it doesn't matter that the sorting destroys the relationship between 
		//stateVertexCounts and states
		std::sort(states.begin()+1, states.end()-1);

		const std::size_t stateSize = states.size();

		FinalColumnVector final;
		InitialRowVector initial;
		constructMatricesDense(transitionMatrix, final, initial, gridDimension, states, nonZeroCount);

		/*Eigen version
		LargeIntMatrix products(transitionMatrix);
		for(int i = 0; i < gridDimension-2; i++)
		{
			products *= transitionMatrix;
		}
		return initial * products * final;
		*/


		//basic FFLAS version
		/*
		LargeIntMatrix products(transitionMatrix), scratchMatrix;
		typedef FFPACK::UnparametricField<unsigned long long> fflasField;
		fflasField field;
		for(int i = 0; i < gridDimension-2; i++)
		{
			FFLAS::fgemm(field, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, stateSize, stateSize, stateSize, 1ULL, &(products(0,0)), stateSize, &(transitionMatrix(0, 0)), stateSize, 0ULL, &(scratchMatrix(0,0)), stateSize);
			scratchMatrix.swap(products);
		}
		return initial * products * final;
		*/
		
		//complicated EIGEN version
		int finalPower = gridDimension - 1;
		if(finalPower == 1)
		{
			return initial * transitionMatrix * final;
		}
		else
		{
			int currentResultPower = 0;
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
			int currentSquarePower = 1;

			LargeDenseIntMatrix squared(transitionMatrix);

			while(currentResultPower != finalPower)
			{
				squared *= squared;
				currentSquarePower <<= 1;
				if(finalPower & currentSquarePower)
				{
					result *= squared;
					currentResultPower += currentSquarePower;
				}
			}
			return initial * result * final;
		}
	}
	mpz_class countSubgraphsMultiThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount)
	{
		if(gridDimension > 32)
		{
			throw std::runtime_error("Parameter gridDimension for function countSubgraphsMultiThreaded must be less than 33");
		}
		return countSubgraphsSingleThreaded(gridDimension, transitionMatrix, nonZeroCount);
	}
}
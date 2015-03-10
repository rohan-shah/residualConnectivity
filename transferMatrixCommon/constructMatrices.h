#ifndef CONSTRUCT_MATRICES_HEADER_GUARD
#define CONSTRUCT_MATRICES_HEADER_GUARD
#include "countSubgraphsCommon.h"
#include "states.h"
namespace discreteGermGrain
{
	void constructMatricesDense(LargeDenseIntMatrix& outputTransitionMatrix, FinalColumnVector& outputFinal, InitialRowVector& outputInitial, const transferStates& states, std::size_t& nonZeroCount);
	void constructMatricesSparse(LargeSparseIntMatrix& outputTransitionMatrix, FinalColumnVector& outputFinal, InitialRowVector& outputInitial, const transferStates& states, std::size_t& nonZeroCount);
}
#endif

#ifndef CONSTRUCT_MATRICES_HEADER_GUARD
#define CONSTRUCT_MATRICES_HEADER_GUARD
#include "countSubgraphsCommon.h"
namespace discreteGermGrain
{
	void constructMatricesDense(LargeDenseIntMatrix& outputTransitionMatrix, FinalColumnVector& outputFinal, InitialRowVector& outputInitial, int gridDimension, const std::vector<unsigned long long>& states, std::size_t& nonZeroCount);
	void constructMatricesSparse(LargeSparseIntMatrix& outputTransitionMatrix, FinalColumnVector& outputFinal, InitialRowVector& outputInitial, int gridDimension, const std::vector<unsigned long long>& states, std::size_t& nonZeroCount);
}
#endif

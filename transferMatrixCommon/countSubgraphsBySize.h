#ifndef COUNT_SUBGRAPHS_BY_SIZE_HEADER_GUARD
#define COUNT_SUBGRAPHS_BY_SIZE_HEADER_GUARD
#include <vector>
#include "countSubgraphsCommon.h"
namespace discreteGermGrain
{
	void countSubgraphsBySizeMultiThreaded(mpz_class* counts, int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount);
	void countSubgraphsBySizeSingleThreaded(mpz_class* counts, int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount);
}
#endif
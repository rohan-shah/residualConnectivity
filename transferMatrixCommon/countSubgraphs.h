#ifndef COUNT_SUBGRAPHS_HEADER_GUARD
#define COUNT_SUBGRAPHS_HEADER_GUARD
#include <vector>
#include "countSubgraphsCommon.h"
namespace discreteGermGrain
{
	mpz_class countSubgraphsMultiThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount);
	mpz_class countSubgraphsSingleThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount);
}
#endif
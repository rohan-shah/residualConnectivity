#ifndef COUNT_SUBGRAPHS_HEADER_GUARD
#define COUNT_SUBGRAPHS_HEADER_GUARD
#include <vector>
#include "countSubgraphsCommon.h"
namespace residualConnectivity
{
	class countSubgraphsLogger
	{
	public:
		virtual void beginComputeStates(){}
		virtual void endComputeStates(const transferStates& states){}
		virtual void beginConstructDenseMatrices(){}
		virtual void endConstructDenseMatrices(LargeDenseIntMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount){}
		virtual void beginMultiplications(){}
		virtual void completedMultiplicationStep(int completed, int total){}
		virtual void endMultiplications(){}
	};
	void constructMatricesDense(LargeDenseIntMatrix& outputTransitionMatrix, FinalColumnVector& outputFinal, InitialRowVector& outputInitial, const transferStates& states, std::size_t& nonZeroCount);
	mpz_class countSubgraphsMultiThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsLogger* logger);
	mpz_class countSubgraphsSingleThreaded(int gridDimension, LargeDenseIntMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsLogger* logger);
}
#endif

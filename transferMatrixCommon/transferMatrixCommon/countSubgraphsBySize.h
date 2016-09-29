#ifndef COUNT_SUBGRAPHS_BY_SIZE_HEADER_GUARD
#define COUNT_SUBGRAPHS_BY_SIZE_HEADER_GUARD
#include <vector>
#include "countSubgraphsCommon.h"
#include "customizedEigen.h"
#include "states.h"
namespace residualConnectivity
{
	class countSubgraphsBySizeLogger
	{
	public:
		virtual void beginComputeStates(){}
		virtual void endComputeStates(const transferStates& states){}
		virtual void beginConstructDenseMatrices(){}
		virtual void endConstructDenseMatrices(TransitionMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount){}
		virtual void beginMultiplications(){}
		virtual void completedMultiplicationStep(int completed, int total){}
		virtual void endMultiplications(){}
		virtual void beginCheckFinalStates(){}
		virtual void endCheckFinalStates(){}
	};
#ifdef USE_OPENMP
	void countSubgraphsBySizeMultiThreaded(mpz_class* counts, int gridDimension, TransitionMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsBySizeLogger* logger);
#endif
	void countSubgraphsBySizeSingleThreaded(mpz_class* counts, int gridDimension, TransitionMatrix& transitionMatrix, std::size_t& nonZeroCount, countSubgraphsBySizeLogger* logger);
}
#endif

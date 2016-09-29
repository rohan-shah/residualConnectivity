#include "countSubgraphsCommon.h"
#include "constructMatrices.h"
namespace residualConnectivity
{
	class transferMatrixUnequalProbabilitiesLogger
	{
	public:
		virtual void beginComputeStates(){}
		virtual void endComputeStates(const transferStates& states){}
		virtual void beginConstructSparseMatrices(){}
		virtual void endConstructSparseMatrices(LargeSparseIntMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount){}
		virtual void beginMultiplications(){}
		virtual void completedMultiplicationStep(int completed, int total){}
		virtual void endMultiplications(){}
		virtual void beginCheckFinalStates(){}
		virtual void endCheckFinalStates(){}
	};
	typedef ::Eigen::Matrix<mpfr_class, -1, -1, ::Eigen::RowMajor, -1, -1> LargeDenseFloatMatrix;
	void transferMatrixUnequalProbabilities(mpfr_class& result, int gridDimension, LargeSparseIntMatrix& transitionMatrix, transferMatrixUnequalProbabilitiesLogger* logger, LargeDenseFloatMatrix& probabilities);
}

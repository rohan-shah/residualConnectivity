#include "transferMatrixUnequalProbabilities.h"
#include "transferMatrixCommon/unequalProbabilities.h"
namespace residualConnectivity
{
	class transferMatrixUnequalProbabilitiesRLogger : public transferMatrixUnequalProbabilitiesLogger
	{
	public:
		virtual void beginComputeStates()
		{
			Rcpp::Rcout << "Computing states...." << std::endl;
		}
		virtual void endComputeStates(const transferStates& states)
		{
			Rcpp::Rcout << "Finished computing states, " << states.getStates().size() << " states found. " << std::endl;
		}
		virtual void beginConstructSparseMatrices()
		{
			Rcpp::Rcout << "Beginning construction of transition matrix..." << std::endl;
		}
		virtual void endConstructSparseMatrices(LargeSparseIntMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount)
		{
			Rcpp::Rcout << "Finished constructing transition matrix, " << nonZeroTransitionCount << " / " << transitionMatrix.size()*transitionMatrix.size() << " transitions are possible." << std::endl;
		}
		virtual void completedMultiplicationStep(int completed, int total)
		{
			Rcpp::Rcout << "Completed multiplication step " << completed << " / " << total << "." <<std::endl;
		}
		virtual void endMultiplications()
		{
			Rcpp::Rcout << "Finished multiplication steps." << std::endl;
		}
		virtual void beginCheckFinalStates()
		{
			Rcpp::Rcout << "Checking valid final states." << std::endl;
		}
		virtual void endCheckFinalStates()
		{
			Rcpp::Rcout << "Finished checking valid final states. " << std::endl;
		}
	};
}
SEXP transferMatrixUnequalProbabilities(SEXP probabilities_sexp)
{
BEGIN_RCPP
	Rcpp::NumericMatrix probabilities;
	try
	{
		probabilities = Rcpp::as<Rcpp::NumericMatrix>(probabilities_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input probabilities must be a numeric matrix");
	}
	if(probabilities.nrow() != probabilities.ncol())
	{
		throw std::runtime_error("Input probabilities matrix must be square");
	}
	residualConnectivity::mpfr_class result;
	residualConnectivity::LargeSparseIntMatrix transitionMatrix;
	residualConnectivity::LargeDenseFloatMatrix probabilities_eigen(probabilities.nrow(), probabilities.nrow());
	for(int row = 0; row < probabilities.nrow(); row++)
	{
		for(int column = 0; column < probabilities.nrow(); column++)
		{
			probabilities_eigen(row, column) = probabilities(row, column);
		}
	}
	residualConnectivity::transferMatrixUnequalProbabilitiesRLogger logger;
	residualConnectivity::transferMatrixUnequalProbabilities(result, probabilities.nrow(), transitionMatrix, &logger, probabilities_eigen);
	return Rcpp::wrap(result.str());
END_RCPP
}


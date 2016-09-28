#include "transferMatrixIdentical.h"
#include <boost/scoped_array.hpp>
#include "countSubgraphs.h"
#include "countSubgraphsBySize.h"
#include "countSubgraphsCommon.h"
#include "constructMatrices.h"
#include "includeMPFRResidualConnectivity.h"
namespace residualConnectivity
{
	class transferMatrixLogger : public countSubgraphsBySizeLogger, public countSubgraphsLogger
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
		virtual void beginConstructDenseMatrices()
		{
			Rcpp::Rcout << "Beginning construction of transition matrix..." << std::endl;
		}
		virtual void endConstructDenseMatrices(LargeDenseIntMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount)
		{
			Rcpp::Rcout << "Finished constructing transition matrix, " << nonZeroTransitionCount << " / " << transitionMatrix.innerSize()*transitionMatrix.innerSize() << " transitions are possible." << std::endl;
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
SEXP transferMatrixIdentical(SEXP size_sexp)
{
BEGIN_RCPP
	int size;
	try
	{
		size = Rcpp::as<int>(size_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input size must be an integer");
	}
	std::vector<residualConnectivity::mpz_class> counts(size*size+1, 0);
	std::size_t nonZeroCount = 0;
	residualConnectivity::TransitionMatrix transitionMatrix;
	residualConnectivity::transferMatrixLogger logger;
	residualConnectivity::countSubgraphsBySizeMultiThreaded(&(counts[0]), size, transitionMatrix, nonZeroCount, &logger);

	std::vector<std::string> sizesAsStrings;
	std::transform(counts.begin(), counts.end(), std::back_inserter(sizesAsStrings), [](const residualConnectivity::mpz_class& x){return x.str();});
	return Rcpp::wrap(sizesAsStrings);
END_RCPP
}

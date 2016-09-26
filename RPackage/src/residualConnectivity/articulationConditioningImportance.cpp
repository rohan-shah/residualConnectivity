#include "monteCarloMethods/articulationConditioningImportance.h"
#include "Rcpp.h"
#include "articulationConditioningImportance.h"
#include "graphInterface.h"
#include "ROutputObject.h"
SEXP articulationConditioningImportance(SEXP importanceProbabilities_sexp, SEXP graph_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp, SEXP verbose_sexp, SEXP finalStepSampleSize_sexp)
{
BEGIN_RCPP
	//convert number of samples
	double n_double;
	try
	{
		n_double = Rcpp::as<double>(n_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input n to a number");
	}
	long n;
	if(std::abs(n_double - std::round(n_double)) > 1e-3)
	{
		throw std::runtime_error("Input n must be an integer");
	}
	n = (long)std::round(n_double);

	double initialRadius_double;
	try
	{
		initialRadius_double = Rcpp::as<double>(initialRadius_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input initialRadius to a number");
	}
	long initialRadius;
	if(std::abs(initialRadius_double - std::round(initialRadius_double)) > 1e-3)
	{
		throw std::runtime_error("Input initialRadius must be an integer");
	}
	initialRadius = (long)std::round(initialRadius_double);

	bool verbose = Rcpp::as<bool>(verbose_sexp);

	std::vector<double> importanceProbabilities;
	try
	{
		importanceProbabilities = Rcpp::as<std::vector<double> >(importanceProbabilities_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input importanceProbabilities must be a numeric vector");
	}

	int finalStepSampleSize;
	try
	{
		finalStepSampleSize = Rcpp::as<int>(finalStepSampleSize_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input finalStepSampleSize must be an integer");
	}

	//convert seed
	int seed;
	try
	{
		seed = Rcpp::as<int>(seed_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input seed must be an integer");
	}
	boost::mt19937 randomSource;
	randomSource.seed(seed);

	residualConnectivity::context contextObj = graphInterface(graph_sexp, probability_sexp);
	ROutputObject output;
	residualConnectivity::articulationConditioningImportanceArgs args(contextObj, randomSource, output);
	args.finalStepSampleSize = finalStepSampleSize;
	args.n = (int)n;
	args.initialRadius = (int)initialRadius;
	args.verbose = verbose;
	args.importanceProbabilities = importanceProbabilities;
	randomSource.seed(seed);

	residualConnectivity::articulationConditioningImportance(args);
	double result = args.estimate.convert_to<double>();
	return Rcpp::wrap(result);
END_RCPP
}

#include "monteCarloMethods/articulationConditioning.h"
#include "Rcpp.h"
#include "articulationConditioning.h"
#include "graphInterface.h"
#include "ROutputObject.h"
SEXP articulationConditioning(SEXP graph_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp, SEXP verbose_sexp, SEXP nLastStep_sexp)
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

	long nLastStep;
	try
	{
		nLastStep = Rcpp::as<long>(nLastStep_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input nLastStep must be a number");
	}
	boost::mt19937 randomSource;
	randomSource.seed(seed);

	residualConnectivity::context contextObj = graphInterface(graph_sexp, probability_sexp);
	ROutputObject output;
	residualConnectivity::articulationConditioningArgs args(contextObj, randomSource, output);
	args.n = (int)n;
	args.initialRadius = (int)initialRadius;
	args.verbose = verbose;
	args.nLastStep = nLastStep;
	randomSource.seed(seed);

	residualConnectivity::articulationConditioning(args);
	double result = args.estimate.convert_to<double>();
	return Rcpp::wrap(result);
END_RCPP
}

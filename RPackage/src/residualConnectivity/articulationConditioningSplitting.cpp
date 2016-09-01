#include "monteCarloMethods/articulationConditioningSplitting.h"
#include "Rcpp.h"
#include "articulationConditioningResampling.h"
#include "graphInterface.h"
#include "ROutputObject.h"
SEXP articulationConditioningSplitting(SEXP graph_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp, SEXP splittingFactors_sexp)
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

	std::vector<float> splittingFactors;
	try
	{
		splittingFactors = Rcpp::as<std::vector<float> >(splittingFactors_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input splittingFactors must be a numeric vector");
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
	residualConnectivity::articulationConditioningSplittingArgs args(contextObj, randomSource, output);
	args.n = (int)n;
	args.initialRadius = (int)initialRadius;
	args.splittingFactors = splittingFactors;
	randomSource.seed(seed);

	residualConnectivity::articulationConditioningSplitting(args);
	Rcpp::List result = Rcpp::List::create(Rcpp::Named("estimate") = args.estimate.convert_to<double>(), Rcpp::Named("levelProbabilities") = Rcpp::wrap(args.levelProbabilities));
	
	return result;
END_RCPP
}

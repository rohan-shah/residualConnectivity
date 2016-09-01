#include "monteCarloMethods/conditionalMC.h"
#include "Rcpp.h"
#include "conditionalMC.h"
#include "graphInterface.h"
SEXP conditionalMC(SEXP graph_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
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
	residualConnectivity::conditionalMCArgs args(contextObj, randomSource);
	args.n = n;
	randomSource.seed(seed);

	residualConnectivity::conditionalMC(args);
	double result = args.estimate.convert_to<double>();
	return Rcpp::wrap(result);
END_RCPP
}

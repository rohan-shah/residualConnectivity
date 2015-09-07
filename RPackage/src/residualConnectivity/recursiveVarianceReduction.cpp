#include "monteCarloMethods/recursiveVarianceReduction.h"
#include "Rcpp.h"
#include "recursiveVarianceReduction.h"
#include "graphInterface.h"
#include "graphType.h"
SEXP recursiveVarianceReduction(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp, graphType type)
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

	discreteGermGrain::Context context = graphInterface(graph_sexp, vertexCoordinates_sexp, probability_sexp, type);
	boost::mt19937 randomSource;
	randomSource.seed(seed);
	discreteGermGrain::recursiveVarianceReductionArgs args(context, randomSource);
	args.n = n;

	discreteGermGrain::recursiveVarianceReduction(args);
	return Rcpp::wrap(args.estimate.convert_to<double>());
END_RCPP
}
SEXP recursiveVarianceReduction_igraph(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return recursiveVarianceReduction(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, IGRAPH);
}
SEXP recursiveVarianceReduction_graphAM(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return recursiveVarianceReduction(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, GRAPHAM);
}
SEXP recursiveVarianceReduction_graphNEL(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return recursiveVarianceReduction(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, GRAPHNEL);
}

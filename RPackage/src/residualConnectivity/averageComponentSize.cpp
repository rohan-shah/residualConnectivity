#include "averageComponentSize.h"
#include "monteCarloMethods/averageComponentSize.h"
#include "graphConvert.h"
#include "graphInterface.h"
SEXP averageComponentSize(SEXP graph_sexp, SEXP probabilities_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	int n;
	try
	{
		n = Rcpp::as<int>(n_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input n must be an integer");
	}

	int seed;
	try
	{
		seed = Rcpp::as<int>(seed_sexp);
	}
	catch(...)
	{
		throw std::runtime_error("Input seed must be a number");
	}

	residualConnectivity::context::inputGraph graph;
	std::vector<residualConnectivity::context::vertexPosition> vertexCoordinates;
	graphConvert(graph_sexp, graph, vertexCoordinates);

	residualConnectivity::context contextObj = graphInterface(graph_sexp, probabilities_sexp);
	boost::mt19937 randomSource;
	randomSource.seed(seed);
	std::vector<residualConnectivity::mpfr_class> estimates;
	residualConnectivity::averageComponentSizeArgs args(contextObj, estimates, randomSource);
	args.n = n;
	residualConnectivity::averageComponentSize(args);

	std::vector<double> estimatesAsDouble;
	std::transform(estimates.begin(), estimates.end(), std::back_inserter(estimatesAsDouble), [](residualConnectivity::mpfr_class& x){return x.convert_to<double>();});
	return Rcpp::wrap(estimatesAsDouble);
}

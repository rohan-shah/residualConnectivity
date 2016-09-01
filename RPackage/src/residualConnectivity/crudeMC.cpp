#include "monteCarloMethods/crudeMC.h"
#include "Rcpp.h"
#include "crudeMC.h"
#include "graphInterface.h"
#include "graphType.h"
#include "graphConvert.h"
SEXP crudeMC(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp, graphType type)
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
	residualConnectivity::context::inputGraph graph;
	std::vector<residualConnectivity::context::vertexPosition> vertexCoordinates;
	graphConvert(graph_sexp, graph, vertexCoordinates);

	residualConnectivity::context contextObj = graphInterface(graph_sexp, vertexCoordinates_sexp, probability_sexp, type);
	boost::mt19937 randomSource;
	randomSource.seed(seed);
	residualConnectivity::crudeMCArgs args(contextObj, randomSource);
	args.n = n;

	std::size_t result = residualConnectivity::crudeMC(args);
	return Rcpp::wrap((double)result / (double)n);
END_RCPP
}
SEXP crudeMC_igraph(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return crudeMC(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, IGRAPH);
}
SEXP crudeMC_graphAM(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return crudeMC(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, GRAPHAM);
}
SEXP crudeMC_graphNEL(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
	return crudeMC(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, seed_sexp, GRAPHNEL);
}

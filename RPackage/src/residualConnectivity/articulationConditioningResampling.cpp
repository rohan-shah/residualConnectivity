#include "monteCarloMethods/articulationConditioningResampling.h"
#include "Rcpp.h"
#include "articulationConditioningResampling.h"
#include "graphInterface.h"
#include "graphType.h"
#include "ROutputObject.h"
SEXP articulationConditioningResampling(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp, graphType type)
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

	discreteGermGrain::Context context = graphInterface(graph_sexp, vertexCoordinates_sexp, probability_sexp, type);
	discreteGermGrain::observationTree tree(&context, (int)initialRadius);
	ROutputObject output;
	discreteGermGrain::articulationConditioningResamplingArgs args(context, randomSource, tree, output);
	args.n = (int)n;
	args.initialRadius = (int)initialRadius;
	randomSource.seed(seed);

	discreteGermGrain::articulationConditioningResampling(args);
	double result = args.estimate.convert_to<double>();
	return Rcpp::wrap(result);
END_RCPP
}
SEXP articulationConditioningResampling_graphAM(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp)
{
	return articulationConditioningResampling(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, initialRadius_sexp, seed_sexp, GRAPHAM);
}
SEXP articulationConditioningResampling_igraph(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp)
{
	return articulationConditioningResampling(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, initialRadius_sexp, seed_sexp, IGRAPH);
}
SEXP articulationConditioningResampling_graphNEL(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP initialRadius_sexp, SEXP seed_sexp)
{
	return articulationConditioningResampling(graph_sexp, vertexCoordinates_sexp, probability_sexp, n_sexp, initialRadius_sexp, seed_sexp, GRAPHNEL);
}

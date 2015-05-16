#include "crudeMCLib.h"
#include "Rcpp.h"
#include "crudeMCInterfaces.h"
SEXP crudeMC_igraph(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, SEXP n_sexp, SEXP seed_sexp)
{
BEGIN_RCPP
	//Convert graph object
	Rcpp::List graph;
	try
	{
		graph = Rcpp::as<Rcpp::List>(graph_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input graph to a list");
	}
	int nVertices = Rcpp::as<int>(graph(0));
	bool directed = Rcpp::as<bool>(graph(1));

	//Convert probability
	double probability;
	try
	{
		probability = Rcpp::as<double>(probability_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input probability to a number");
	}
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

	//Convert vertex coordinates
	Rcpp::NumericMatrix vertexCoordinates_matrix;
	try
	{
		vertexCoordinates_matrix = Rcpp::as<Rcpp::NumericMatrix>(vertexCoordinates_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input vertexCoordinates must be an integer");
	}
	//change format
	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexCoordinates(new std::vector<discreteGermGrain::Context::vertexPosition>());
	std::vector<discreteGermGrain::Context::vertexPosition>& vertexCoordinatesRef = *vertexCoordinates;
	vertexCoordinatesRef.reserve(vertexCoordinates_matrix.nrow());
	for(int i = 0; i < nVertices; i++)
	{
		vertexCoordinatesRef.push_back(discreteGermGrain::Context::vertexPosition(vertexCoordinates_matrix(i, 0), vertexCoordinates_matrix(i, 1)));
	}

	Rcpp::IntegerVector edgesVertex1 = Rcpp::as<Rcpp::IntegerVector>(graph(2));
	Rcpp::IntegerVector edgesVertex2 = Rcpp::as<Rcpp::IntegerVector>(graph(3));
	if(edgesVertex1.size() != edgesVertex2.size())
	{
		throw std::runtime_error("Internal error");
	}

	//Construct graph
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph(new discreteGermGrain::Context::inputGraph(nVertices));
	discreteGermGrain::Context::inputGraph& graphRef = *boostGraph;
	for(int i = 0; i < edgesVertex1.size(); i++)
	{
		boost::add_edge(edgesVertex1(i), edgesVertex2(i), graphRef);
	}

	//Construct default vertex ordering
	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(int i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back(i);
	}

	discreteGermGrain::Context context(boostGraph, defaultOrdering, vertexCoordinates, probability);
	discreteGermGrain::crudeMCArgs args(context);
	args.n = n;
	args.randomSource.seed(1);

	std::size_t result = discreteGermGrain::crudeMC(args);
	return Rcpp::wrap((int)result);
END_RCPP
}
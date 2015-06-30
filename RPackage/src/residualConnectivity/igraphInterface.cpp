#include "igraphInterface.h"
discreteGermGrain::Context igraphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp)
{
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

	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph = igraphConvert(graph_sexp);
	std::size_t nVertices = boost::num_vertices(*boostGraph);

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
	for(std::size_t i = 0; i < nVertices; i++)
	{
		vertexCoordinatesRef.push_back(discreteGermGrain::Context::vertexPosition((discreteGermGrain::Context::vertexPosition::first_type)vertexCoordinates_matrix((int)i, 0), (discreteGermGrain::Context::vertexPosition::second_type)vertexCoordinates_matrix((int)i, 1)));
	}

	//Construct default vertex ordering
	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(std::size_t i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back((int)i);
	}

	discreteGermGrain::Context context(boostGraph, defaultOrdering, vertexCoordinates, probability);
	return context;
}
boost::shared_ptr<discreteGermGrain::Context::inputGraph> igraphConvert(SEXP graph_sexp)
{
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
	Rcpp::IntegerVector edgesVertex1 = Rcpp::as<Rcpp::IntegerVector>(graph(2));
	Rcpp::IntegerVector edgesVertex2 = Rcpp::as<Rcpp::IntegerVector>(graph(3));
	if(edgesVertex1.size() != edgesVertex2.size())
	{
		throw std::runtime_error("Internal error");
	}
	if(directed)
	{
		throw std::runtime_error("Input graph must be undirected");
	}

	//Construct graph
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph(new discreteGermGrain::Context::inputGraph(nVertices));
	discreteGermGrain::Context::inputGraph& graphRef = *boostGraph;
	for(int i = 0; i < edgesVertex1.size(); i++)
	{
		boost::add_edge(edgesVertex1(i), edgesVertex2(i), graphRef);
	}
	return boostGraph;
}

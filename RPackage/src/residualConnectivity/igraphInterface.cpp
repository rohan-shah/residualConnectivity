#include "igraphInterface.h"
discreteGermGrain::Context igraphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp)
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
	if(directed)
	{
		throw std::runtime_error("Input graph must be undirected");
	}

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
	return context;
}

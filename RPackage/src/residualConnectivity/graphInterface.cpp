#include "graphInterface.h"
#include "igraphInterface.h"
#include "graphAMInterface.h"
#include "graphNELInterface.h"
discreteGermGrain::Context graphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, graphType type)
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

	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph;
	if(type == GRAPHAM) boostGraph = graphAMConvert(graph_sexp);
	else if(type == GRAPHNEL) boostGraph = graphNELConvert(graph_sexp);
	else if(type == IGRAPH) boostGraph = igraphConvert(graph_sexp);
	else throw std::runtime_error("Internal error");

	std::size_t nVertices = boost::num_vertices(*boostGraph);


	//Convert vertex coordinates
	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexCoordinates;
	if(Rf_isNull(vertexCoordinates_sexp) == 0)
	{
		Rcpp::NumericMatrix vertexCoordinates_matrix;
		try
		{
			vertexCoordinates_matrix = Rcpp::as<Rcpp::NumericMatrix>(vertexCoordinates_sexp);
		}
		catch(Rcpp::not_compatible&)
		{
			throw std::runtime_error("Input vertexCoordinates must be a numeric matrix");
		}
		//change format
		vertexCoordinates = boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> >(new std::vector<discreteGermGrain::Context::vertexPosition>());
		std::vector<discreteGermGrain::Context::vertexPosition>& vertexCoordinatesRef = *vertexCoordinates;
		vertexCoordinatesRef.reserve(vertexCoordinates_matrix.nrow());
		for(std::size_t i = 0; i < nVertices; i++)
		{
			vertexCoordinatesRef.push_back(discreteGermGrain::Context::vertexPosition((discreteGermGrain::Context::vertexPosition::first_type)vertexCoordinates_matrix((int)i, 0), (discreteGermGrain::Context::vertexPosition::second_type)vertexCoordinates_matrix((int)i, 1)));
		}
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
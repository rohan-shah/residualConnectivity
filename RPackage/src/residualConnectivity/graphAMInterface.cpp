#include "graphAMInterface.h"
discreteGermGrain::Context graphAMInterface(SEXP graph_sexp, SEXP probability_sexp)
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

	Rcpp::S4 graph_s4;
	try
	{
		graph_s4 = Rcpp::as<Rcpp::S4>(graph_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input graph must be an S4 object");
	}
	if(Rcpp::as<std::string>(graph_s4.attr("class")) != "graphAM")
	{
		throw std::runtime_error("Input graph must have class graphAM");
	}
	
	Rcpp::RObject adjMat_obj;
	try
	{
		adjMat_obj = Rcpp::as<Rcpp::RObject>(graph_s4.slot("adjMat"));
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Error extracting slot adjMat of input graph");
	}

	Rcpp::NumericMatrix adjMat;
	try
	{
		adjMat = Rcpp::as<Rcpp::NumericMatrix>(adjMat_obj);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Error extracting slot adjMat of input graph");
	}
	
	int nVertices = adjMat.nrow();
	if(adjMat.ncol() != nVertices)
	{
		throw std::runtime_error("Slot adjMat of input graph must be a square matrix");
	}

	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph(new discreteGermGrain::Context::inputGraph(nVertices));
	discreteGermGrain::Context::inputGraph& boostGraphRef = *boostGraph;
	for(int i = 0; i < nVertices; i++)
	{
		for(int j = 0; j < nVertices; j++)
		{
			if(adjMat(i, j) > 0)
			{
				boost::add_edge((std::size_t)i, (std::size_t)j, boostGraphRef);
			}
		}
	}

	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(int i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back(i);
	}

	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexCoordinates(new std::vector<discreteGermGrain::Context::vertexPosition>());
	std::vector<discreteGermGrain::Context::vertexPosition>& vertexCoordinatesRef = *vertexCoordinates;
	vertexCoordinatesRef.reserve(nVertices);
	float increment = (float)(2.0*M_PI/nVertices);
	for(int i = 0; i < nVertices; i++)
	{
		float angle = i * increment;
		vertexCoordinatesRef.push_back(discreteGermGrain::Context::vertexPosition(cosf(angle), sinf(angle)));
	}

	discreteGermGrain::Context context(boostGraph, defaultOrdering, vertexCoordinates, probability);
	return context;
}
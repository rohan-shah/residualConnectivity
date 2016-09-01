#include "graphInterface.h"
#include "igraphInterface.h"
#include "graphAMInterface.h"
#include "graphNELInterface.h"
#include "graphConvert.h"
residualConnectivity::context graphInterface(SEXP graph_sexp, SEXP probabilities_sexp)
{
	//Convert probability
	std::vector<mpfr_class> probabilities;
	Rcpp::NumericVector probabilities_R;
	try
	{
		probabilities_R = Rcpp::as<Rcpp::NumericVector>(probabilities_sexp);
		std::transform(probabilities_R.begin(), probabilities_R.end(), std::back_inserter(probabilities), [](double x){return mpfr_class(x); });
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input probability to a number");
	}

	boost::shared_ptr<residualConnectivity::context::inputGraph> boostGraph(new residualConnectivity::context::inputGraph());
	boost::shared_ptr<std::vector<residualConnectivity::context::vertexPosition> > vertexCoordinates(new std::vector<residualConnectivity::context::vertexPosition>());
	graphConvert(graph_sexp, *boostGraph.get(), *vertexCoordinates.get()); 
	std::size_t nVertices = boost::num_vertices(*boostGraph);

	//Construct default vertex ordering
	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(std::size_t i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back((int)i);
	}

	residualConnectivity::context contextObj(boostGraph, defaultOrdering, vertexCoordinates, probabilities);
	return contextObj;
}
residualConnectivity::context graphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probabilities_sexp, graphType type)
{
	//Convert probability
	std::vector<mpfr_class> probabilities;
	Rcpp::NumericVector probabilities_R;
	try
	{
		probabilities_R = Rcpp::as<Rcpp::NumericVector>(probabilities_sexp);
		std::transform(probabilities_R.begin(), probabilities_R.end(), std::back_inserter(probabilities), [](double x){return mpfr_class(x); });
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input probability to a number");
	}

	boost::shared_ptr<residualConnectivity::context::inputGraph> boostGraph(new residualConnectivity::context::inputGraph());
	if(type == GRAPHAM) graphAMConvert(graph_sexp, *boostGraph.get());
	else if(type == GRAPHNEL) graphNELConvert(graph_sexp, *boostGraph.get());
	else if(type == IGRAPH) igraphConvert(graph_sexp, *boostGraph.get());
	else throw std::runtime_error("Internal error");

	std::size_t nVertices = boost::num_vertices(*boostGraph);


	//Convert vertex coordinates
	boost::shared_ptr<std::vector<residualConnectivity::context::vertexPosition> > vertexCoordinates;
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
		vertexCoordinates = boost::shared_ptr<std::vector<residualConnectivity::context::vertexPosition> >(new std::vector<residualConnectivity::context::vertexPosition>());
		std::vector<residualConnectivity::context::vertexPosition>& vertexCoordinatesRef = *vertexCoordinates;
		vertexCoordinatesRef.reserve(vertexCoordinates_matrix.nrow());
		for(std::size_t i = 0; i < nVertices; i++)
		{
			vertexCoordinatesRef.push_back(residualConnectivity::context::vertexPosition((residualConnectivity::context::vertexPosition::first_type)vertexCoordinates_matrix((int)i, 0), (residualConnectivity::context::vertexPosition::second_type)vertexCoordinates_matrix((int)i, 1)));
		}
	}

	//Construct default vertex ordering
	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(std::size_t i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back((int)i);
	}

	residualConnectivity::context contextObj(boostGraph, defaultOrdering, vertexCoordinates, probabilities);
	return contextObj;
}

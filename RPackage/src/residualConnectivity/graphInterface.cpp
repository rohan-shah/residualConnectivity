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
	if(probabilities.size() != nVertices && probabilities.size() != 1)
	{
		throw std::runtime_error("Input probabilities must contain a single value, or a value for each vertex");
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

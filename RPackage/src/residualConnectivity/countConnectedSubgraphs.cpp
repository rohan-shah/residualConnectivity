#include "countConnectedSubgraphs.h"
#include "exactMethods/exhaustiveSearch.h"
#include "graphConvert.h"
#include <sstream>
SEXP countConnectedSubgraphs(SEXP graph)
{
BEGIN_RCPP
	boost::shared_ptr<residualConnectivity::context::inputGraph> boostGraph(new residualConnectivity::context::inputGraph());
	std::vector<residualConnectivity::context::vertexPosition> vertexCoordinates;
	graphConvert(graph, *boostGraph.get(), vertexCoordinates);
	std::string message;
	std::vector<residualConnectivity::counterType> sizeCounters;
	bool result = residualConnectivity::exhaustiveSearch(*boostGraph, sizeCounters, message);
	if(!result)
	{
		throw std::runtime_error(message.c_str());
	}

	Rcpp::Function convertToBigZ("as.bigz");
	Rcpp::S4 retVal("exactCounts");
	Rcpp::CharacterVector stringRepresentations((int)sizeCounters.size());
	for(std::size_t i = 0; i < sizeCounters.size(); i++)
	{
		std::stringstream ss; ss << sizeCounters[i];
		stringRepresentations(i) = ss.str();
	}
	retVal.slot("counts") = convertToBigZ(stringRepresentations);
	return retVal;
END_RCPP
}

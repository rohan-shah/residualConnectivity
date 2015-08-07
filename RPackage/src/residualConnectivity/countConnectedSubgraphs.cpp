#include "countConnectedSubgraphs.h"
#include "exhaustiveSearchLib.h"
#include "graphConvert.h"
#include "graphType.h"
#include <sstream>
SEXP countConnectedSubgraphs(SEXP graph, graphType type)
{
BEGIN_RCPP
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph = graphConvert(graph, type);
	std::string message;
	std::vector<discreteGermGrain::counterType> sizeCounters;
	bool result = discreteGermGrain::exhaustiveSearch(*boostGraph, sizeCounters, message);
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
SEXP countConnectedSubgraphs_igraph(SEXP graph)
{
	return countConnectedSubgraphs(graph, IGRAPH);
}
SEXP countConnectedSubgraphs_graphNEL(SEXP graph)
{
	return countConnectedSubgraphs(graph, GRAPHNEL);
}
SEXP countConnectedSubgraphs_graphAM(SEXP graph)
{
	return countConnectedSubgraphs(graph, GRAPHAM);
}
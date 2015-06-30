#include "exhaustiveSearchInterfaces.h"
#include "exhaustiveSearchLib.h"
#include "igraphInterface.h"
#include <sstream>
SEXP exhaustiveSearch_igraph(SEXP graph)
{
BEGIN_RCPP
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph = igraphConvert(graph);
	std::string message;
	std::vector<discreteGermGrain::counterType> sizeCounters;
	bool result = discreteGermGrain::exhaustiveSearch(*boostGraph, sizeCounters, message);
	if(!result)
	{
		throw std::runtime_error(message.c_str());
	}

	Rcpp::Function convertToBigZ("as.bigz");
	Rcpp::S4 retVal("exhaustiveSubgraphs");
	Rcpp::CharacterVector stringRepresentations((int)sizeCounters.size());
	for(std::size_t i = 0; i < sizeCounters.size(); i++)
	{
		std::stringstream ss; ss << sizeCounters[i];
		stringRepresentations(i) = ss.str();
	}
	retVal.slot(".S3Class") = convertToBigZ(stringRepresentations);
	return retVal;
END_RCPP
}

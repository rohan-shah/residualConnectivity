#ifndef ARGUMENTS_MPIR_HEADER_GUARD
#define ARGUMENTS_MPIR_HEADER_GUARD
#include <boost/program_options.hpp>
#include "includeMPFRResidualConnectivity.h"
#include "context.h"
namespace residualConnectivity
{
	bool readProbabilityString(boost::program_options::variables_map& variableMap, std::vector<mpfr_class>& out, std::string& message);
	bool readGraph(boost::program_options::variables_map& variableMap, context::inputGraph& graph, std::vector<context::vertexPosition>& vertexPositions, std::vector<int>& ordering, std::string& message);
	bool readSingleProbabilityString(boost::program_options::variables_map& variableMap, mpfr_class& out, std::string& message);
}
#endif

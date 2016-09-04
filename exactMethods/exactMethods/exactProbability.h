#ifndef EXACT_PROBABILITY_HEADER_GUARD
#define EXACT_PROBABILITY_HEADER_GUARD
#include "context.h"
#include <vector>
#include "includeMPFRResidualConnectivity.h"
#include <string>
namespace residualConnectivity
{
	typedef long long counterType;
	bool exactProbability(const context::inputGraph& inputGraph, std::vector<mpfr_class>& probabilities, mpfr_class& result, std::string& error);
}
#endif

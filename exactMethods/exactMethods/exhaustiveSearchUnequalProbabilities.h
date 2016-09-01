#ifndef EXHAUSTIVE_SEACRH_UNEQUAL_PROBABILITIES_HEADER_GUARD
#define EXHAUSTIVE_SEACRH_UNEQUAL_PROBABILITIES_HEADER_GUARD
#include "context.h"
#include <vector>
#include "includeNumerics.h"
#include <string>
namespace residualConnectivity
{
	bool exhaustiveSearchUnequalProbabilities(const context::inputGraph& inputGraph, std::vector<mpfr_class>& probabilities, mpfr_class& result, std::string& error);
}
#endif

#ifndef OPTIMAL_STATE_INDEPENDENCE_IMPORTANCE_DENSITY_HEADER_GUARD
#define OPTIMAL_STATE_INDEPENDENCE_IMPORTANCE_DENSITY_HEADER_GUARD
#include <vector>
#include "includeMPFRResidualConnectivity.h"
namespace residualConnectivity
{
	void optimalStateIndependentImportance(std::vector<mpz_class>& graphCounts, std::vector<mpfr_class>& probabilities, std::vector<mpfr_class>& bestImportanceProbabilities);
}
#endif

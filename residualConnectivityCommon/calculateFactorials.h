#ifndef CALCULATE_FACTORIALS_HEADER_GUARD
#define CALCULATE_FACTORIALS_HEADER_GUARD
#include "includeMPFRResidualConnectivity.h"
#include <vector>
namespace residualConnectivity
{
	void calculateFactorials(std::vector<mpz_class>& factorials, int upperLimit);
}
#endif

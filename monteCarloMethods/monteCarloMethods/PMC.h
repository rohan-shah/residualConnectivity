#ifndef PMC_LIB_HEADER_GUARD
#define PMC_LIB_HEADER_GUARD
#include "context.h"
#include <vector>
#include "includeMPFRResidualConnectivity.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct pmcArguments
	{
	public:
		pmcArguments(const context::inputGraph& graph, boost::mt19937& randomSource);
		const context::inputGraph& graph;
		boost::mt19937& randomSource;
		int n;
		std::vector<unsigned long long> counts;
	};
	bool PMC(pmcArguments& args);
}
#endif

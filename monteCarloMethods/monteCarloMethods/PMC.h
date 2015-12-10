#ifndef PMC_LIB_HEADER_GUARD
#define PMC_LIB_HEADER_GUARD
#include "Context.h"
#include <vector>
#include "includeNumerics.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct pmcArguments
	{
	public:
		pmcArguments(const Context::inputGraph& graph, boost::mt19937& randomSource);
		const Context::inputGraph& graph;
		boost::mt19937& randomSource;
		int n;
		std::vector<unsigned long long> counts;
	};
	bool PMC(pmcArguments& args);
}
#endif
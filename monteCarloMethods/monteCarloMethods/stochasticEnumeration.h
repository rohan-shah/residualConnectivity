#ifndef STOCHASTIC_ENUMERATION_LIB_HEADER_GUARD
#define STOCHASTIC_ENUMERATION_LIB_HEADER_GUARD
#include <boost/random/mersenne_twister.hpp>
#include "context.h"
#include "includeMPFRResidualConnectivity.h"
namespace residualConnectivity
{
	struct stochasticEnumerationArgs
	{
	public:
		stochasticEnumerationArgs(const context::inputGraph& graph, boost::mt19937& randomSource)
			:graph(graph), randomSource(randomSource)
			{}
		const context::inputGraph& graph;
		boost::mt19937& randomSource;
		int nPermutations;
		int vertexCount;
		int n;
		mpfr_class estimate;
		std::string message;
	};
	bool stochasticEnumeration1(stochasticEnumerationArgs& args);
	bool stochasticEnumeration2(stochasticEnumerationArgs& args);
}
#endif

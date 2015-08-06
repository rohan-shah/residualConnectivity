#ifndef STOCHASTIC_ENUMERATION_LIB_HEADER_GUARD
#define STOCHASTIC_ENUMERATION_LIB_HEADER_GUARD
#include <boost/random/mersenne_twister.hpp>
#include "Context.h"
#include "includeNumerics.h"
namespace discreteGermGrain
{
	struct stochasticEnumerationArgs
	{
	public:
		stochasticEnumerationArgs(const Context::inputGraph& graph, boost::mt19937& randomSource)
			:graph(graph), randomSource(randomSource)
			{}
		const Context::inputGraph& graph;
		boost::mt19937& randomSource;
		int vertexCount;
		int n;
		mpfr_class estimate;
		std::string message;
	};
	bool stochasticEnumeration(stochasticEnumerationArgs& args);
}
#endif
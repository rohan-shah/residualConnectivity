#include "monteCarloMethods/averageComponentSize.h"
namespace residualConnectivity
{
	void averageComponentSize()
	{
		const context& contextObj = args.contextObj;
		const context::inputGraph& graph = contextObj.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);
		std::vector<mpfr_class>& probabilities = contextObj.getProbabilities();

		std::vector<unsigned long long> integerSum(nVertices, 0);
		for(int i = 0; i < n; i++)
		{
			observation obs(args.contextObj, args.randomSource);
			isSingleComponentAllOn(args.contextObj, obs.getState(), scratchMemory, stack);
		}

		result.clear();
		std::transform(integerSum.begin(), integerSum.end(), std::make_back_inserter(result), [n](unsigned long long x){return mpfr_class(x)/mpfr_class(n);});
	}
}

#include "monteCarloMethods/averageComponentSize.h"
#include "observation.h"
namespace residualConnectivity
{
	void averageComponentSize(averageComponentSizeArgs& args)
	{
		const context& contextObj = args.contextObj;
		const context::inputGraph& graph = contextObj.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);

		std::vector<unsigned long long> integerSum(nVertices, 0);
		std::vector<int> componentSizes(nVertices);
		for(int i = 0; i < args.n; i++)
		{
			observation obs(args.contextObj, args.randomSource);
			isSingleComponentAllOn(args.contextObj, obs.getState(), args.scratchMemory, args.stack);
			const vertexState* state = obs.getState();
			std::fill(componentSizes.begin(), componentSizes.end(), 0);
			for(int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
			{
				if(state[vertexCounter].state & ON_MASK) componentSizes[args.scratchMemory[vertexCounter]]++;
			}
			for(int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
			{
				if(state[vertexCounter].state & ON_MASK) integerSum[vertexCounter] += componentSizes[args.scratchMemory[vertexCounter]];
			}
		}
		int n = args.n;
		args.result.clear();
		std::transform(integerSum.begin(), integerSum.end(), std::back_inserter(args.result), [n](unsigned long long x){return mpfr_class(mpfr_class(x)/mpfr_class(n));});
	}
}

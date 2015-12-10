#include "observation.h"
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/crudeMC.h"
namespace residualConnectivity
{
	std::size_t crudeMC(crudeMCArgs& args)
	{
		std::vector<observation> observations;
		std::vector<int> scratchMemory;
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;

		//The cumulative states, in case we decide to use the --splitting option
		std::vector<vertexState> cumulativeStates(args.context.nVertices());
		for(int i = 0; i < args.n; i++)
		{
			observation obs(args.context, args.randomSource);
			if(isSingleComponentAllOn(args.context, obs.getState(), scratchMemory, stack))
			{
				observations.push_back(std::move(obs));
			}
		}
		return observations.size();
	}
}

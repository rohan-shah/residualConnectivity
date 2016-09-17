#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <algorithm>
#include "empiricalDistribution.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningSameCountImportance.h"
#include "subObs/articulationConditioningSameCountImportance.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "bridges.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/articulationConditioningSameCountImportance.h"
namespace residualConnectivity
{
	void articulationConditioningSameCountImportance(articulationConditioningSameCountImportanceArgs& args)
	{
		const context& contextObj = args.contextObj;
		boost::mt19937& randomSource = args.randomSource;
		const std::vector<double>* importanceProbabilities = &args.importanceProbabilities;
		int n = args.n;
		args.estimate = 0;

		std::vector<int> connectedComponents;
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
		boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType filteredGraphStack;
		::residualConnectivity::subObs::articulationConditioningSameCountImportanceConstructorType getSubObsHelper(connectedComponents, stack, filteredGraphStack);
		::residualConnectivity::obs::articulationConditioningSameCountImportanceConstructorType getObsHelper;

		for(int i = 0; i < n; i++)
		{
			obs::articulationConditioningSameCountImportance observation(contextObj, importanceProbabilities, randomSource);
			for(int currentRadius = args.initialRadius; currentRadius >= 1; currentRadius--)
			{
				subObs::articulationConditioningSameCountImportance subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningSameCountImportance>::get(observation, currentRadius, getSubObsHelper);
				if(currentRadius == 1)
				{
					if(subObs.isPotentiallyConnected()) args.estimate += subObs.getWeight()*subObs.estimateRadius1(randomSource, args.finalStepSampleSize, connectedComponents, stack);
				}
				else if(subObs.isPotentiallyConnected())
				{
					observation = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioningSameCountImportance>::get(subObs, randomSource, getObsHelper);
				}
			}
		}
		args.estimate /= n;
	}
}


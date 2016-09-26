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
	void articulationConditioningSameCountImportance(articulationConditioningImportanceArgs& args)
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

		std::vector<mpfr_class> sums(args.initialRadius+1, 0);
		for(int i = 0; i < n; i++)
		{
			obs::articulationConditioningSameCountImportance observation(contextObj, importanceProbabilities, randomSource);
			for(int currentRadius = args.initialRadius; currentRadius >= 1; currentRadius--)
			{
				subObs::articulationConditioningSameCountImportance subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningSameCountImportance>::get(observation, currentRadius, getSubObsHelper);
				if(currentRadius == 1)
				{
					if(subObs.isPotentiallyConnected())
					{
						sums[currentRadius] += subObs.getWeight();
						mpfr_class extraPart = subObs.estimateRadius1(randomSource, args.finalStepSampleSize, connectedComponents, stack);
						args.estimate += subObs.getWeight()*extraPart;
						sums[currentRadius-1] += subObs.getWeight()*extraPart;

					}
				}
				else if(subObs.isPotentiallyConnected())
				{
					observation = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioningSameCountImportance>::get(subObs, randomSource, getObsHelper);
					sums[currentRadius] += subObs.getWeight();
				}
				else break;
			}
		}
		if(args.verbose)
		{
			for(int currentRadius = args.initialRadius; currentRadius >= 0; currentRadius--)
			{
				args.output << "Radius " << currentRadius << ", probability " << mpfr_class(sums[currentRadius]/args.n).str(10, std::ios_base::dec) << outputObject::endl;
			}
		}
		args.estimate /= n;
	}
}


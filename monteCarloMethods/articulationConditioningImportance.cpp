#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <algorithm>
#include "empiricalDistribution.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningImportance.h"
#include "subObs/articulationConditioningImportance.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "bridges.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/articulationConditioningImportance.h"
namespace residualConnectivity
{
	void articulationConditioningImportance(articulationConditioningImportanceArgs& args)
	{
		const context& contextObj = args.contextObj;
		boost::mt19937& randomSource = args.randomSource;
		const std::vector<double>* importanceProbabilities = &args.importanceProbabilities;
		int n = args.n;
		args.estimate = 1;

		std::vector<int> connectedComponents;
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
		boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType filteredGraphStack;
		::residualConnectivity::subObs::withWeightImportanceConstructorType getSubObsHelper(connectedComponents, stack, filteredGraphStack);
		getSubObsHelper.importanceProbabilities = &args.importanceProbabilities;
		::residualConnectivity::obs::withWeightImportanceConstructorType getObsHelper;

		std::vector<mpfr_class> transitionProbabilities(args.initialRadius+1, 0);
		std::vector<subObs::articulationConditioningImportance> subObservations, copiedSubObservations;
		mpfr_class currentSum = 0, previousSum;
		for(int i = 0; i < n; i++)
		{
			obs::articulationConditioningImportance observation(contextObj, randomSource, importanceProbabilities);
			subObs::articulationConditioningImportance subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningImportance>::get(observation, args.initialRadius, getSubObsHelper);
			if(subObs.isPotentiallyConnected())
			{
				currentSum += subObs.getWeight();
				subObservations.emplace_back(std::move(subObs));
			}
		}
		transitionProbabilities[args.initialRadius] = currentSum/args.n;
		args.estimate *= transitionProbabilities[args.initialRadius];
		if(args.verbose)
		{
			args.output << "Radius " << args.initialRadius << ", probability " << transitionProbabilities[args.initialRadius].str(10, std::ios_base::dec) << outputObject::endl;
		}
		int multiple = n / (int)subObservations.size();
		copiedSubObservations.clear();
		for(int i = 0; i < multiple; i++)
		{
			copiedSubObservations.insert(copiedSubObservations.begin(), subObservations.begin(), subObservations.end());
		}
		int remaining = n - (int)copiedSubObservations.size();
		std::vector<int> indices;
		for(int i = 0; i < (int)subObservations.size(); i++) indices.push_back(i);
		boost::random_shuffle(indices);
		previousSum = currentSum*multiple;
		for(int i = 0; i < remaining; i++)
		{
			copiedSubObservations.push_back(subObservations[indices[i]]);
			previousSum += copiedSubObservations.back().getWeight();
		}
		copiedSubObservations.swap(subObservations);
		mpfr_class lastRadiusSum = 0;
		for(int currentRadius = args.initialRadius-1; currentRadius >= 1; currentRadius--)
		{
			currentSum = 0;
			copiedSubObservations.clear();
			for(int i = 0; i < n; i++)
			{
				::residualConnectivity::obs::articulationConditioningImportance observation = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioningImportance>::get(subObservations[i], randomSource, getObsHelper);
				subObs::articulationConditioningImportance subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningImportance>::get(observation, currentRadius, getSubObsHelper);
				if(currentRadius == 1)
				{
					if(subObs.isPotentiallyConnected())
					{
						mpfr_class extraPart = subObs.estimateRadius1(randomSource, args.finalStepSampleSize, connectedComponents, stack);
						currentSum += subObs.getWeight();
						lastRadiusSum += subObs.getWeight() * extraPart;
					}
				}
				else if(subObs.isPotentiallyConnected())
				{
					currentSum += subObs.getWeight();
					copiedSubObservations.emplace_back(std::move(subObs));
				}
			}
			transitionProbabilities[currentRadius] = currentSum / previousSum;
			args.estimate *= transitionProbabilities[currentRadius];
			if(currentRadius != 1)
			{
				int multiple = n / (int)subObservations.size();
				copiedSubObservations.clear();
				for(int i = 0; i < multiple; i++)
				{
					copiedSubObservations.insert(copiedSubObservations.begin(), subObservations.begin(), subObservations.end());
				}
				int remaining = n - (int)copiedSubObservations.size();
				std::vector<int> indices;
				for(int i = 0; i < (int)subObservations.size(); i++) indices.push_back(i);
				boost::random_shuffle(indices);
				previousSum = currentSum*multiple;
				for(int i = 0; i < remaining; i++)
				{
					copiedSubObservations.push_back(subObservations[indices[i]]);
					previousSum += copiedSubObservations.back().getWeight();
				}
				copiedSubObservations.swap(subObservations);
				if(args.verbose)
				{
					args.output << "Radius " << currentRadius << ", probability " << transitionProbabilities[currentRadius].str(10, std::ios_base::dec) << outputObject::endl;
				}
			}
		}
		transitionProbabilities[0] = lastRadiusSum / currentSum;
		args.estimate *= transitionProbabilities[0];
		if(args.verbose)
		{
			args.output << "Radius 1, probability " << transitionProbabilities[1].str(10, std::ios_base::dec) << outputObject::endl;
			args.output << "Radius 0, probability " << transitionProbabilities[0].str(10, std::ios_base::dec) << outputObject::endl;
		}
	}
}


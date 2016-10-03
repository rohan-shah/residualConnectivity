#include <boost/archive/basic_archive.hpp>
#include "monteCarloMethods/usingBiconnectedSplitting.h"
#include <algorithm>
#include <iostream>
#include "empiricalDistribution.h"
#include "observation.h"
#include "obs/usingBiconnectedComponents.h"
#include "subObs/usingBiconnectedComponents.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
//#include "bridges.hpp"
#include <fstream>
#include "obs/getSubObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
#include "argumentsMPFR.h"
namespace residualConnectivity
{
	struct stepInputs
	{
		stepInputs(context const& contextObj, const std::vector<float>& splittingFactors)
			:contextObj(contextObj), splittingFactors(splittingFactors)
		{}
		context const& contextObj;
		const std::vector<float>& splittingFactors;
		int initialRadius;
		long n;
	};
	struct stepOutputs
	{
		stepOutputs(std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents>& subObservations, std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>& observations, boost::mt19937& randomSource, outputObject& outputStream)
			:subObservations(subObservations), observations(observations), randomSource(randomSource), outputStream(outputStream)
		{}
		std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents>& subObservations;
		std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>& observations;
		boost::mt19937& randomSource;
		mpfr_class sumLastStepProbabilities;
		int nNonZeroProbabilities;
		outputObject& outputStream;
	};
	void stepOne(const stepInputs& inputs, stepOutputs& outputs)
	{
		outputs.sumLastStepProbabilities = 0;
		outputs.nNonZeroProbabilities = 0;
#ifdef USE_OPENMP
		boost::mt19937 perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = outputs.randomSource();
#endif

		float sptittingFactorRemainder = inputs.splittingFactors[inputs.initialRadius-1] - floor(inputs.splittingFactors[inputs.initialRadius-1]);
		int splittingFactorInteger = (int)floor(inputs.splittingFactors[inputs.initialRadius-1]);
		//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
#ifdef USE_OPENMP
		#pragma omp parallel
#endif
		{
			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(boost::num_vertices(inputs.contextObj.getGraph()));
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
			//used to calculate the splitting factor (which is random)
			boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);
			
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			mpfr_class sumLastStepProbabilitiesThisThread = 0;
			int nNonZeroProbabilities = 0;
			#pragma omp for
#else
			boost::mt19937& perThreadSource = outputs.randomSource;
			mpfr_class& sumLastStepProbabilitiesThisThread = outputs.sumLastStepProbabilities;
			int& nNonZeroProbabilities = outputs.nNonZeroProbabilities;
#endif
			for(int j = 0; j < (int)outputs.subObservations.size(); j++)
			{
				//number of observations which the current observation is split into
				int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
				//get out the child observations
				mpfr_class probability = outputs.subObservations[j].estimateRadius1(perThreadSource, nThisObservation, connectedComponents, stack);
				sumLastStepProbabilitiesThisThread += probability;
				if(probability != 0) nNonZeroProbabilities++;
			}
#ifdef USE_OPENMP
			#pragma omp barrier 
			#pragma omp critical
			{
				outputs.sumLastStepProbabilities += sumLastStepProbabilitiesThisThread;
				outputs.nNonZeroProbabilities += nNonZeroProbabilities;
			}
#endif
		}
	}
	void stepsExceptOne(stepInputs& inputs, stepOutputs& outputs)
	{
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937 perThreadSeeds[100];
		for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
		std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents> nextSetObservations;
		std::vector<int> nextStepPotentiallyConnectedIndices;
		//Loop over the splitting steps (the different nested events)
		for(int i = 1; i < inputs.initialRadius/*+1*/; i++)
		{
			float sptittingFactorRemainder = inputs.splittingFactors[i-1] - floor(inputs.splittingFactors[i-1]);
			int splittingFactorInteger = (int)floor(inputs.splittingFactors[i-1]);

			//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
			int generated = -1;
			nextSetObservations.clear();
			nextStepPotentiallyConnectedIndices.clear();
			
			//loop over the various sample paths
#ifdef USE_OPENMP
			#pragma omp parallel
#endif
			{
				//vector that we re-use to avoid allocations
				std::vector<int> connectedComponents(boost::num_vertices(inputs.contextObj.getGraph()));
				//stack for depth first search
				boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
				boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType filteredGraphStack;
				//used to calculate the splitting factor (which is random)
				boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);

				::residualConnectivity::subObs::withWeightConstructorType getSubObsHelper(connectedComponents, stack, filteredGraphStack);
				::residualConnectivity::obs::withWeightConstructorType getObsHelper;
#ifdef USE_OPENMP
				//per-thread random number generation
				boost::mt19937 perThreadSource;
				perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);

				#pragma omp for
#else
				boost::mt19937& perThreadSource = outputs.randomSource;
#endif
				for(int j = 0; j < (int)outputs.subObservations.size(); j++)
				{
					//number of observations which the current observation is split into
					int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
					//get out the current observation
					const ::residualConnectivity::subObs::usingBiconnectedComponents& currentObs = outputs.subObservations[j];
					for(int k = 0; k < nThisObservation; k++)
					{
						::residualConnectivity::obs::usingBiconnectedComponents obs = ::residualConnectivity::subObs::getObservation< ::residualConnectivity::subObs::usingBiconnectedComponents>::get(currentObs, perThreadSource, getObsHelper);
						::residualConnectivity::subObs::usingBiconnectedComponents subObs = ::residualConnectivity::obs::getSubObservation< ::residualConnectivity::obs::usingBiconnectedComponents>::get(obs, inputs.initialRadius - i, getSubObsHelper);
#ifdef USE_OPENMP
						#pragma omp critical
#endif
						{
							generated++;
							if(subObs.isPotentiallyConnected())
							{
								nextSetObservations.push_back(std::move(subObs));
							}
						}
					}
				}
			}
			outputs.outputStream << "Finished splitting step " << i << " / " << inputs.initialRadius << ", " << nextSetObservations.size() << " / " << generated+1 << " observations continuing" << outputObject::endl;
			outputs.subObservations.swap(nextSetObservations);
		}
	}
	void doCrudeMCStep(stepInputs& inputs, stepOutputs& outputs)
	{
		int generated = -1;
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937 perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = outputs.randomSource();
		#pragma omp parallel
#endif
		{
			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(boost::num_vertices(inputs.contextObj.getGraph()));
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
			boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType filteredGraphStack;
			::residualConnectivity::subObs::withWeightConstructorType helper(connectedComponents, stack, filteredGraphStack);
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			#pragma omp for
#else 
			boost::mt19937& perThreadSource = outputs.randomSource;
#endif
			for(int i = 0; i < inputs.n; i++)
			{
				::residualConnectivity::obs::usingBiconnectedComponents obs(inputs.contextObj, perThreadSource);
				::residualConnectivity::subObs::usingBiconnectedComponents subObs(::residualConnectivity::obs::getSubObservation< ::residualConnectivity::obs::usingBiconnectedComponents>::get(obs, inputs.initialRadius, helper));
#ifdef USE_OPENMP
				#pragma omp critical
#endif
				{
					generated++;
					if(subObs.isPotentiallyConnected())
					{
						outputs.subObservations.push_back(std::move(subObs));
					}
				}
			}
		}
	}
	void usingBiconnectedSplitting(usingBiconnectedSplittingArgs& args)
	{
		int n = args.n;
		std::vector<float>& splittingFactors = args.splittingFactors;
		context const& contextObj = args.contextObj;
		int initialRadius = args.initialRadius;
		boost::mt19937& randomSource = args.randomSource;
		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm
		std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents> subObservations;
		std::vector< ::residualConnectivity::obs::usingBiconnectedComponents> observations;

		stepInputs inputs(contextObj, splittingFactors);
		inputs.initialRadius = initialRadius;
		inputs.n = n;

		stepOutputs outputs(subObservations, observations, randomSource, args.outputStream);

		doCrudeMCStep(inputs, outputs);
		float crudeMCProbability = ((float)observations.size()) / n;
		args.outputStream << "Retaining " << subObservations.size() << " / " << n << " observations from crude MC step" << outputObject::endl;

		if(initialRadius != 0)
		{
			//This does nothing for the case initialRadius = 1
			stepsExceptOne(inputs, outputs);
			
			//When the radius is 1 we use a different algorithm
			stepOne(inputs, outputs);
			args.outputStream << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << outputs.nNonZeroProbabilities << " / " << outputs.subObservations.size()  << " observations had non-zero probability" << outputObject::endl;

			mpfr_class averageLastStep = outputs.sumLastStepProbabilities / outputs.subObservations.size();
			args.outputStream << "Average probability at last step was " << averageLastStep.str() << outputObject::endl;
			mpfr_class totalSampleSize = n;
			for(std::vector<float>::iterator i = splittingFactors.begin(); i != splittingFactors.end()-1; i++) totalSampleSize *= *i;
			args.estimate = outputs.sumLastStepProbabilities / totalSampleSize;
			//Swap the vectors over, so that the results are always stored in finalObservations. Just in case we want to 
			//revert back to using the allExceptOne algorithm for all the steps. 
		}
		else
		{
			args.estimate = crudeMCProbability;
		}
	}
}

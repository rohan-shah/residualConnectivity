#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <algorithm>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioning.h"
#include "subObs/articulationConditioning.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "bridges.hpp"
#include "obs/getSubObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/articulationConditioning.h"
namespace residualConnectivity
{
	struct stepInputs
	{
		stepInputs(context const& contextObj)
			:contextObj(contextObj), initialRadius(-1), n(-1), nLastStep(-1)
		{}
		context const& contextObj;
		int initialRadius;
		long n;
		long nLastStep;
		bool verbose;
	};
	struct stepOutputs
	{
		stepOutputs(std::vector<::residualConnectivity::subObs::articulationConditioning>& subObservations, std::vector<::residualConnectivity::obs::articulationConditioning>& observations, boost::mt19937& randomSource, outputObject& output)
			:subObservations(subObservations), observations(observations), randomSource(randomSource), output(output)
		{}
		std::vector<::residualConnectivity::subObs::articulationConditioning>& subObservations;
		std::vector<::residualConnectivity::obs::articulationConditioning>& observations;
		boost::mt19937& randomSource;
		outputObject& output;
		std::vector<std::ptrdiff_t> aliasMethodTemporary1, aliasMethodTemporary2;
		std::vector<std::pair<double, std::ptrdiff_t> > aliasMethodTemporary3;
		mpfr_class sumProbabilities;
	};
	void stepLast(stepInputs& inputs, stepOutputs& outputs)
	{
		outputs.sumProbabilities = 0;
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
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

			::residualConnectivity::subObs::withWeightConstructorType getSubObsHelper(connectedComponents, stack, filteredGraphStack);
			::residualConnectivity::obs::withWeightConstructorType getObsHelper;
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			mpfr_class sumProbabilitiesThisThread = 0;

			#pragma omp for
#else
			boost::mt19937& perThreadSource = outputs.randomSource;
			mpfr_class& sumProbabilitiesThisThread = outputs.sumProbabilities;
#endif
			for(int j = 0; j < (int)outputs.subObservations.size(); j++)
			{
				//get out the current observation
				sumProbabilitiesThisThread += outputs.subObservations[j].getWeight()*outputs.subObservations[j].estimateRadius1(perThreadSource, inputs.nLastStep, connectedComponents, stack);
			}
#ifdef USE_OPENMP
			#pragma omp barrier
			#pragma omp critical
			{
				outputs.sumProbabilities += sumProbabilitiesThisThread;
			}
#endif
		}
	}
	void stepsExceptFirst(stepInputs& inputs, stepOutputs& outputs)
	{
		std::vector<double> resamplingProbabilities;
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
		std::vector<::residualConnectivity::subObs::articulationConditioning> nextSetObservations;
		std::vector<int> nextStepPotentiallyConnectedIndices;
		//Loop over the splitting steps (the different nested events)
		for(int i = 1; i < inputs.initialRadius; i++)
		{
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
					//get out the current observation
					const ::residualConnectivity::subObs::articulationConditioning& currentObs = outputs.subObservations[j];
					::residualConnectivity::obs::articulationConditioning obs = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioning>::get(currentObs, perThreadSource, getObsHelper);
					::residualConnectivity::subObs::articulationConditioning subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioning>::get(obs, inputs.initialRadius - i, getSubObsHelper);
#ifdef USE_OPENMP
					#pragma omp critical
#endif
					{
						if(subObs.isPotentiallyConnected())
						{
							nextSetObservations.push_back(std::move(subObs));
						}
					}
				}
			}
			if(inputs.verbose) outputs.output << "Finished step " << i << " / " << inputs.initialRadius << outputObject::endl;
			outputs.subObservations.clear();
			mpfr_class sum = 0;
			resamplingProbabilities.clear();
			for(std::vector<::residualConnectivity::subObs::articulationConditioning>::iterator j = nextSetObservations.begin(); j != nextSetObservations.end(); j++)
			{
				sum += j->getWeight();
				resamplingProbabilities.push_back(j->getWeight().convert_to<double>());
			}
			if(sum == 0) return;
			mpfr_class averageWeight = sum / inputs.n;
			aliasMethod::aliasMethod alias(resamplingProbabilities, sum.convert_to<double>(), outputs.aliasMethodTemporary1, outputs.aliasMethodTemporary2, outputs.aliasMethodTemporary3);
			for(int j = 0; j < inputs.n; j++)
			{
				int index = (int)alias(outputs.randomSource);
				outputs.subObservations.push_back(nextSetObservations[index].copyWithWeight(averageWeight));
			}
		}
	}
	void doCrudeMCStep(stepInputs& inputs, stepOutputs& outputs)
	{
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
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
				::residualConnectivity::obs::articulationConditioning obs(inputs.contextObj, perThreadSource);
				::residualConnectivity::subObs::articulationConditioning subObs(::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioning>::get(obs, inputs.initialRadius, helper));
#ifdef USE_OPENMP
				#pragma omp critical
#endif
				{
					if(subObs.isPotentiallyConnected())
					{
						outputs.subObservations.push_back(std::move(subObs));
					}
				}
			}
		}
		std::vector<double> resamplingProbabilities;
		mpfr_class sum = 0;
		for(std::vector<::residualConnectivity::subObs::articulationConditioning>::iterator i = outputs.subObservations.begin(); i != outputs.subObservations.end(); i++)
		{
			sum += i->getWeight();
			resamplingProbabilities.push_back(i->getWeight().convert_to<double>());
		}
		mpfr_class averageWeight = sum / inputs.n;
		aliasMethod::aliasMethod alias(resamplingProbabilities, sum.convert_to<double>(), outputs.aliasMethodTemporary1, outputs.aliasMethodTemporary2, outputs.aliasMethodTemporary3);
		std::vector<::residualConnectivity::subObs::articulationConditioning> nextSubObservations;
		for(int i = 0; i < inputs.n; i++)
		{
			int index = (int)alias(outputs.randomSource);
			nextSubObservations.push_back(outputs.subObservations[index].copyWithWeight(averageWeight));
		}
		outputs.subObservations.swap(nextSubObservations);
	}
	void articulationConditioning(articulationConditioningArgs& args)
	{
		std::vector<::residualConnectivity::subObs::articulationConditioning> subObservations;
		std::vector<::residualConnectivity::obs::articulationConditioning> observations;

		stepInputs inputs(args.contextObj);
		inputs.initialRadius = args.initialRadius;
		inputs.n = args.n;
		inputs.verbose = args.verbose;
		inputs.nLastStep = args.nLastStep;

		stepOutputs outputs(subObservations, observations, args.randomSource, args.output);

		doCrudeMCStep(inputs, outputs);

		stepsExceptFirst(inputs, outputs);

		stepLast(inputs, outputs);

		args.estimate =  outputs.sumProbabilities / args.n;
	}
}


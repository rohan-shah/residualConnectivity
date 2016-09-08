#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <algorithm>
#include "empiricalDistribution.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningSameCount.h"
#include "subObs/articulationConditioningSameCount.h"
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
#include "monteCarloMethods/articulationConditioningSameCount.h"
namespace residualConnectivity
{
	namespace articulationConditioningSameCountPrivate
	{
		struct stepInputs
		{
			stepInputs(context const& contextObj)
				:contextObj(contextObj)
			{}
			context const& contextObj;
			int initialRadius;
			long n;
			bool verbose;
		};
		struct stepOutputs
		{
			stepOutputs(std::vector<::residualConnectivity::subObs::articulationConditioningSameCount>& subObservations, std::vector<::residualConnectivity::obs::articulationConditioningSameCount>& observations, boost::mt19937& randomSource, outputObject& output)
				:subObservations(subObservations), observations(observations), randomSource(randomSource), output(output)
			{}
			std::vector<::residualConnectivity::subObs::articulationConditioningSameCount>& subObservations;
			std::vector<::residualConnectivity::obs::articulationConditioningSameCount>& observations;
			std::vector<int> potentiallyConnectedIndices;
			boost::mt19937& randomSource;
			outputObject& output;
			std::vector<std::ptrdiff_t> aliasMethodTemporary1, aliasMethodTemporary2;
			std::vector<std::pair<double, std::ptrdiff_t> > aliasMethodTemporary3;
		};
		mpfr_class finalStep(const stepInputs& inputs, stepOutputs& outputs, int finalStepCount)
		{
#ifdef USE_OPENMP
			boost::mt19937 perThreadSeeds[100];
			for(int i = 0; i < 100; i++) perThreadSeeds[i] = outputs.randomSource();
#endif
			mpfr_class result = 0;
			//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
#ifdef USE_OPENMP
			#pragma omp parallel
#endif
			{
				std::vector< ::residualConnectivity::obs::articulationConditioningSameCount> observationsThisThread;
				//vector that we re-use to avoid allocations
				std::vector<int> connectedComponents(boost::num_vertices(inputs.contextObj.getGraph()));
				//stack for depth first search
				boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
				mpfr_class sumThisThread = 0;
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
					//get out the child observations
					sumThisThread += outputs.subObservations[j].getWeight()*outputs.subObservations[j].estimateRadius1(perThreadSource, finalStepCount, connectedComponents, stack);
				}
#ifdef USE_OPENMP
				for(int j = 0; j < omp_get_num_threads(); j++)
				{
					#pragma omp barrier 
					#pragma omp critical
					{
						result += sumThisThread;
					}
				}
#else
				result = sumThisThread;
#endif
			}
			return result;
		}
		void stepsExceptFirst(stepInputs& inputs, stepOutputs& outputs)
		{
			std::vector<double> resamplingProbabilities;
#ifdef USE_OPENMP
			//set up per-thread random number generators
			boost::mt19937::result_type perThreadSeeds[100];
			for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
			std::vector<::residualConnectivity::subObs::articulationConditioningSameCount> nextSetObservations;
			std::vector<int> nextStepPotentiallyConnectedIndices;
			//Loop over the splitting steps (the different nested events)
			for(int i = 1; i < inputs.initialRadius; i++)
			{
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

					::residualConnectivity::subObs::articulationConditioningSameCountConstructorType getSubObsHelper(connectedComponents, stack, filteredGraphStack);
					::residualConnectivity::obs::weightAndCountConstructorType getObsHelper;
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
						const ::residualConnectivity::subObs::articulationConditioningSameCount& currentObs = outputs.subObservations[j];
						::residualConnectivity::obs::articulationConditioningSameCount obs = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioningSameCount>::get(currentObs, perThreadSource, getObsHelper);
						::residualConnectivity::subObs::articulationConditioningSameCount subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningSameCount>::get(obs, inputs.initialRadius - i, getSubObsHelper);
#ifdef USE_OPENMP
						#pragma omp critical
#endif
						{
							generated++;
							if(subObs.isPotentiallyConnected())
							{
								nextSetObservations.push_back(std::move(subObs));
								nextStepPotentiallyConnectedIndices.push_back(generated);
							}
						}
					}
				}
				if(inputs.verbose) outputs.output << "Finished splitting step " << i << " / " << inputs.initialRadius << outputObject::endl;
				outputs.subObservations.clear();
				outputs.potentiallyConnectedIndices.clear();
				mpfr_class sum = 0;
				resamplingProbabilities.clear();
				for(std::vector<::residualConnectivity::subObs::articulationConditioningSameCount>::iterator j = nextSetObservations.begin(); j != nextSetObservations.end(); j++)
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
					outputs.potentiallyConnectedIndices.push_back(nextStepPotentiallyConnectedIndices[index]);
				}
			}
		}
		void doCrudeMCStep(stepInputs& inputs, stepOutputs& outputs)
		{
			int generated = -1;
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
				::residualConnectivity::subObs::articulationConditioningSameCountConstructorType helper(connectedComponents, stack, filteredGraphStack);
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
					::residualConnectivity::obs::articulationConditioningSameCount obs(inputs.contextObj, perThreadSource);
					::residualConnectivity::subObs::articulationConditioningSameCount subObs(::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningSameCount>::get(obs, inputs.initialRadius, helper));
#ifdef USE_OPENMP
					#pragma omp critical
#endif
					{
						generated++;
						if(subObs.isPotentiallyConnected())
						{
							outputs.subObservations.push_back(std::move(subObs));
							outputs.potentiallyConnectedIndices.push_back(generated);
						}
					}
				}
			}
			std::vector<double> resamplingProbabilities;
			mpfr_class sum = 0;
			for(std::vector<::residualConnectivity::subObs::articulationConditioningSameCount>::iterator i = outputs.subObservations.begin(); i != outputs.subObservations.end(); i++)
			{
				sum += i->getWeight();
				resamplingProbabilities.push_back(i->getWeight().convert_to<double>());
			}
			if(sum == 0) return;
			mpfr_class averageWeight = sum / inputs.n;
			aliasMethod::aliasMethod alias(resamplingProbabilities, sum.convert_to<double>(), outputs.aliasMethodTemporary1, outputs.aliasMethodTemporary2, outputs.aliasMethodTemporary3);
			std::vector<int> nextPotentiallyConnectedIndices;
			std::vector<::residualConnectivity::subObs::articulationConditioningSameCount> nextSubObservations;
			for(int i = 0; i < inputs.n; i++)
			{
				int index = (int)alias(outputs.randomSource);
				nextSubObservations.push_back(outputs.subObservations[index].copyWithWeight(averageWeight));
				nextPotentiallyConnectedIndices.push_back(outputs.potentiallyConnectedIndices[index]);
			}
			outputs.subObservations.swap(nextSubObservations);
			outputs.potentiallyConnectedIndices.swap(nextPotentiallyConnectedIndices);
		}
	}
	void articulationConditioningSameCount(articulationConditioningSameCountArgs& args)
	{
		const std::size_t nVertices = boost::num_vertices(args.contextObj.getGraph());

		std::vector<::residualConnectivity::subObs::articulationConditioningSameCount> subObservations;
		std::vector<::residualConnectivity::obs::articulationConditioningSameCount> observations;

		articulationConditioningSameCountPrivate::stepInputs inputs(args.contextObj);
		inputs.initialRadius = args.initialRadius;
		inputs.n = args.n;
		inputs.verbose = args.verbose;

		articulationConditioningSameCountPrivate::stepOutputs outputs(subObservations, observations, args.randomSource, args.output);

		articulationConditioningSameCountPrivate::doCrudeMCStep(inputs, outputs);

		articulationConditioningSameCountPrivate::stepsExceptFirst(inputs, outputs);

		args.estimate = finalStep(inputs, outputs, args.finalStepSampleSize);
		args.estimate /= args.n;
	}
}


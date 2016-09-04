#include <algorithm>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include "observation.h"
#include "obs/articulationConditioningForSplitting.h"
#include "subObs/articulationConditioningForSplitting.h"
#include <boost/random/bernoulli_distribution.hpp>
#include "bridges.hpp"
#include "obs/getSubObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/articulationConditioningSplitting.h"
namespace residualConnectivity
{
	namespace articulationConditioningSplittingPrivate
	{
		struct stepInputs
		{
			stepInputs(context const& contextObj, const std::vector<double>& splittingFactors)
				:contextObj(contextObj), splittingFactors(splittingFactors), outputTree(false)
			{}
			context const& contextObj;
			const std::vector<double>& splittingFactors;
			int initialRadius;
			long n;
			bool outputTree;
			bool verbose;
		};
		struct stepOutputs
		{
			stepOutputs(std::vector<::residualConnectivity::subObs::articulationConditioningForSplitting>& subObservations, std::vector<::residualConnectivity::obs::articulationConditioningForSplitting>& observations, boost::mt19937& randomSource, observationTree& tree, std::vector<double>& levelProbabilities, outputObject& output)
				:subObservations(subObservations), observations(observations), randomSource(randomSource), tree(tree), levelProbabilities(levelProbabilities), output(output)
			{}
			std::vector<::residualConnectivity::subObs::articulationConditioningForSplitting>& subObservations;
			std::vector<::residualConnectivity::obs::articulationConditioningForSplitting>& observations;
			std::vector<int> potentiallyConnectedIndices;
			boost::mt19937& randomSource;
			observationTree& tree;
			std::vector<double>& levelProbabilities;
			outputObject& output;
			long totalGenerated;
		};
		void stepOne(const stepInputs& inputs, stepOutputs& outputs)
		{
			outputs.totalGenerated = 0;
#ifdef USE_OPENMP
			boost::mt19937::result_type perThreadSeeds[100];
			for(int i = 0; i < 100; i++) perThreadSeeds[i] = outputs.randomSource();
#endif

			float sptittingFactorRemainder = inputs.splittingFactors[inputs.initialRadius-1] - floor(inputs.splittingFactors[inputs.initialRadius-1]);
			int splittingFactorInteger = (int)floor(inputs.splittingFactors[inputs.initialRadius-1]);
			//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
#ifdef USE_OPENMP
			#pragma omp parallel
#endif
			{
				std::vector<::residualConnectivity::obs::articulationConditioningForSplitting> observationsThisThread;
				std::vector<int> parentIndicesThisThread;
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
				long totalGeneratedThisThread = 0;
				#pragma omp for
#endif
				for(int j = 0; j < (int)outputs.subObservations.size(); j++)
				{
					//number of observations which the current observation is split into
					int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(
#ifdef USE_OPENMP
							perThreadSource
#else
							outputs.randomSource
#endif
							);
					std::size_t previousLength = observationsThisThread.size();
					//get out the child observations
					outputs.subObservations[j].estimateRadius1(
#ifdef USE_OPENMP
							perThreadSource
#else
							outputs.randomSource
#endif
							, nThisObservation, connectedComponents, stack, observationsThisThread);
					std::size_t newLength = observationsThisThread.size();
					parentIndicesThisThread.insert(parentIndicesThisThread.end(), newLength - previousLength, outputs.potentiallyConnectedIndices[j]);
#ifdef USE_OPENMP
					totalThisThreadGenerated += nThisObservation;
#else
					outputs.totalGenerated += nThisObservation;
#endif
				}
#ifdef USE_OPENMP
				for(int j = 0; j < omp_get_num_threads(); j++)
				{
					#pragma omp barrier 
					#pragma omp critical
					{
						if(j == omp_get_thread_num())
						{
							if(inputs.outputTree)
							{
								for(int k = 0; k < observationsThisThread.size(); k++)
								{
									outputs.tree.add(observationsThisThread[k], initialRadius, parentIndicesThisThread[k], true);
								}
							}
							output.observations.insert(output.observations.end(), std::make_move_iterator(observationsThisThread.begin()), std::make_move_iterator(observationsThisThread.end()));
							outputs.totalGenerated += totalGeneratedThisThread;
						}
					}
				}
#else
				if(inputs.outputTree)
				{
					for(std::size_t k = 0; k < observationsThisThread.size(); k++)
					{
						outputs.tree.add(observationsThisThread[k], inputs.initialRadius, parentIndicesThisThread[k], true);
					}
				}
				outputs.observations.swap(observationsThisThread);
#endif
			}
		}
		void stepsExceptOne(stepInputs& inputs, stepOutputs& outputs)
		{
#ifdef USE_OPENMP
			//set up per-thread random number generators
			boost::mt19937::result_type perThreadSeeds[100];
			for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
			std::vector<::residualConnectivity::subObs::articulationConditioningForSplitting> nextSetObservations;
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
					boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;
					subGraphType subGraph;
					//used to calculate the splitting factor (which is random)
					boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);

					::residualConnectivity::subObs::withWeightConstructorType getSubObsHelper(connectedComponents, stack, subGraphStack, subGraph);
					::residualConnectivity::obs::withWeightConstructorType getObsHelper;
#ifdef USE_OPENMP
					//per-thread random number generation
					boost::mt19937 perThreadSource;
					perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);

					#pragma omp for
#endif
					for(int j = 0; j < (int)outputs.subObservations.size(); j++)
					{
						//number of observations which the current observation is split into
#ifdef USE_OPENMP
						int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
#else
						int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(outputs.randomSource);
#endif
						//get out the current observation
						const ::residualConnectivity::subObs::articulationConditioningForSplitting& currentObs = outputs.subObservations[j];
						for(int k = 0; k < nThisObservation; k++)
						{
							::residualConnectivity::obs::articulationConditioningForSplitting obs = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::articulationConditioningForSplitting>::get(currentObs, 
#ifdef USE_OPENMP
									perThreadSource
#else
									outputs.randomSource
#endif
							, getObsHelper);
							::residualConnectivity::subObs::articulationConditioningForSplitting subObs = ::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningForSplitting>::get(obs, inputs.initialRadius - i, getSubObsHelper);
#ifdef USE_OPENMP
							#pragma omp critical
#endif
							{
								if(inputs.outputTree) outputs.tree.add(subObs, i, outputs.potentiallyConnectedIndices[j], subObs.isPotentiallyConnected());
								generated++;
								if(subObs.isPotentiallyConnected())
								{
									nextSetObservations.push_back(std::move(subObs));
									nextStepPotentiallyConnectedIndices.push_back(generated);
								}
							}
						}
					}
				}
				if(inputs.verbose) outputs.output << "Finished splitting step " << i << " / " << inputs.initialRadius << ", " << nextSetObservations.size() << " / " << generated+1 << " observations continuing" << outputObject::endl;
				outputs.levelProbabilities.push_back((double)nextSetObservations.size() / (double)(generated+1));
				outputs.subObservations.swap(nextSetObservations);
				outputs.potentiallyConnectedIndices.swap(nextStepPotentiallyConnectedIndices);
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
				boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;
				subGraphType subGraph;
				::residualConnectivity::subObs::withWeightConstructorType helper(connectedComponents, stack, subGraphStack, subGraph);
#ifdef USE_OPENMP
				//per-thread random number generation
				boost::mt19937 perThreadSource;
				perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
				#pragma omp for
#endif
				for(int i = 0; i < inputs.n; i++)
				{
					::residualConnectivity::obs::articulationConditioningForSplitting obs(inputs.contextObj, 
#ifdef USE_OPENMP
							perThreadSource
#else
							outputs.randomSource
#endif
							);
					::residualConnectivity::subObs::articulationConditioningForSplitting subObs(::residualConnectivity::obs::getSubObservation<::residualConnectivity::obs::articulationConditioningForSplitting>::get(obs, inputs.initialRadius, helper));
#ifdef USE_OPENMP
					#pragma omp critical
#endif
					{
						if(inputs.outputTree) outputs.tree.add(subObs, 0, -1, subObs.isPotentiallyConnected());
						generated++;
						if(subObs.isPotentiallyConnected())
						{
							outputs.subObservations.push_back(std::move(subObs));
							outputs.potentiallyConnectedIndices.push_back(generated);
						}
					}
				}
			}
		}
	}
	void articulationConditioningSplitting(articulationConditioningSplittingArgs& args)
	{
		const context& contextObj = args.contextObj;
		std::vector<double>& splittingFactors = args.splittingFactors;
		std::vector<double>& levelProbabilities = args.levelProbabilities;
		levelProbabilities.clear();
		int initialRadius = args.initialRadius;
		int n = args.n;
		boost::mt19937& randomSource = args.randomSource;
		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm
		std::vector<::residualConnectivity::subObs::articulationConditioningForSplitting> subObservations;
		std::vector<::residualConnectivity::obs::articulationConditioningForSplitting> observations;
		observationTree tree(&contextObj, initialRadius);

		articulationConditioningSplittingPrivate::stepInputs inputs(contextObj, splittingFactors);
		inputs.verbose = args.verbose;
		inputs.initialRadius = initialRadius;
		inputs.n = n;

		articulationConditioningSplittingPrivate::stepOutputs outputs(subObservations, observations, randomSource, tree, levelProbabilities, args.output);
		outputs.totalGenerated = 0;

		articulationConditioningSplittingPrivate::doCrudeMCStep(inputs, outputs);
		if(args.verbose) args.output << "Retaining " << subObservations.size() << " / " << n << " observations from crude MC step" << outputObject::endl;
		levelProbabilities.push_back((double)subObservations.size()/(double)n);

		if(initialRadius != 0)
		{
			//This does nothing for the case initialRadius = 1
			articulationConditioningSplittingPrivate::stepsExceptOne(inputs, outputs);
			
			//When the radius is 1 we use a different algorithm
			articulationConditioningSplittingPrivate::stepOne(inputs, outputs);
			if(args.verbose) args.output << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << observations.size() << " / " << outputs.totalGenerated  << " observations had non-zero probability" << outputObject::endl;
			levelProbabilities.push_back((double)observations.size() / (double)outputs.totalGenerated);

			mpfr_class probabilitySum = 0;
			for(std::vector<::residualConnectivity::obs::articulationConditioningForSplitting>::iterator i = observations.begin(); i != observations.end(); i++)
			{
				probabilitySum += i->getWeight();
			}
			mpfr_class averageLastStep = probabilitySum / outputs.totalGenerated;
			mpfr_class totalSampleSize = n;
			for(std::vector<double>::iterator i = splittingFactors.begin(); i != splittingFactors.end(); i++) totalSampleSize *= *i;
			args.estimate = probabilitySum / totalSampleSize;
			//Swap the vectors over, so that the results are always stored in finalObservations. Just in case we want to 
			//revert back to using the allExceptOne algorithm for all the steps. 
		}
		else
		{
			mpfr_class probabilitySum = 0;
			for(std::vector<::residualConnectivity::subObs::articulationConditioningForSplitting>::iterator i = subObservations.begin(); i != subObservations.end(); i++)
			{
				probabilitySum += i->getWeight();
			}
			args.estimate = probabilitySum / n;
		}
	}
}


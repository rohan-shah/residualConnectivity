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
		bool outputTree;
	};
	struct stepOutputs
	{
		stepOutputs(std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents>& subObservations, std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>& observations, boost::mt19937& randomSource, observationTree& tree, outputObject& outputStream)
			:subObservations(subObservations), observations(observations), randomSource(randomSource), tree(tree), outputStream(outputStream)
		{}
		std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents>& subObservations;
		std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>& observations;
		std::vector<int> potentiallyConnectedIndices;
		boost::mt19937& randomSource;
		observationTree& tree;
		long totalGenerated;
		outputObject& outputStream;
	};
	void stepOne(const stepInputs& inputs, stepOutputs& outputs)
	{
		outputs.totalGenerated = 0;
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
			std::vector< ::residualConnectivity::obs::usingBiconnectedComponents> observationsThisThread;
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
					const ::residualConnectivity::subObs::usingBiconnectedComponents& currentObs = outputs.subObservations[j];
					for(int k = 0; k < nThisObservation; k++)
					{
						::residualConnectivity::obs::usingBiconnectedComponents obs = ::residualConnectivity::subObs::getObservation< ::residualConnectivity::subObs::usingBiconnectedComponents>::get(currentObs, 
#ifdef USE_OPENMP
								perThreadSource
#else
								outputs.randomSource
#endif
						, getObsHelper);
						::residualConnectivity::subObs::usingBiconnectedComponents subObs = ::residualConnectivity::obs::getSubObservation< ::residualConnectivity::obs::usingBiconnectedComponents>::get(obs, inputs.initialRadius - i, getSubObsHelper);
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
			outputs.outputStream << "Finished splitting step " << i << " / " << inputs.initialRadius << ", " << nextSetObservations.size() << " / " << generated+1 << " observations continuing" << outputObject::endl;
			outputs.subObservations.swap(nextSetObservations);
			outputs.potentiallyConnectedIndices.swap(nextStepPotentiallyConnectedIndices);
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
				::residualConnectivity::obs::usingBiconnectedComponents obs(inputs.contextObj, 
#ifdef USE_OPENMP
						perThreadSource
#else
						outputs.randomSource
#endif
						);
				::residualConnectivity::subObs::usingBiconnectedComponents subObs(::residualConnectivity::obs::getSubObservation< ::residualConnectivity::obs::usingBiconnectedComponents>::get(obs, inputs.initialRadius, helper));
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
	void usingBiconnectedSplitting(usingBiconnectedSplittingArgs& args)
	{
		int n = args.n;
		std::vector<float>& splittingFactors = args.splittingFactors;
		context const& contextObj = args.contextObj;
		int initialRadius = args.initialRadius;
		bool outputTree = args.outputTree;
		boost::mt19937& randomSource = args.randomSource;
		observationTree& tree = args.tree;
		bool outputDistribution = args.outputDistribution;
		std::string outputDistributionFile = args.outputDistributionFile;
		std::string outputTreeFile = args.outputTreeFile;
		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm
		std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents> subObservations;
		std::vector< ::residualConnectivity::obs::usingBiconnectedComponents> observations;

		stepInputs inputs(contextObj, splittingFactors);
		inputs.initialRadius = initialRadius;
		inputs.outputTree = outputTree;
		inputs.n = n;

		stepOutputs outputs(subObservations, observations, randomSource, tree, args.outputStream);
		outputs.totalGenerated = 0;

		doCrudeMCStep(inputs, outputs);
		float crudeMCProbability = ((float)observations.size()) / n;
		args.outputStream << "Retaining " << subObservations.size() << " / " << n << " observations from crude MC step" << outputObject::endl;

		if(initialRadius != 0)
		{
			//This does nothing for the case initialRadius = 1
			stepsExceptOne(inputs, outputs);
			
			//When the radius is 1 we use a different algorithm
			stepOne(inputs, outputs);
			args.outputStream << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << observations.size() << " / " << outputs.totalGenerated  << " observations had non-zero probability" << outputObject::endl;

			mpfr_class probabilitySum = 0;
			for(std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>::iterator i = observations.begin(); i != observations.end(); i++)
			{
				probabilitySum += i->getWeight();
			}
			mpfr_class averageLastStep = probabilitySum / outputs.totalGenerated;
			args.outputStream << "Average probability at last step was " << averageLastStep.str() << outputObject::endl;
			mpfr_class totalSampleSize = n;
			for(std::vector<float>::iterator i = splittingFactors.begin(); i != splittingFactors.end(); i++) totalSampleSize *= *i;
			args.estimate = probabilitySum / totalSampleSize;
			//Swap the vectors over, so that the results are always stored in finalObservations. Just in case we want to 
			//revert back to using the allExceptOne algorithm for all the steps. 
		}
		else
		{
			args.estimate = crudeMCProbability;
		}


		if(outputDistribution)
		{
			std::ofstream stream(outputDistributionFile.c_str());
			if(stream.is_open())
			{
				boost::archive::binary_oarchive oarchive(stream);
				empiricalDistribution distribution(true, boost::num_vertices(contextObj.getGraph()), contextObj);
				if(initialRadius == 0)
				{
					for(std::vector< ::residualConnectivity::subObs::usingBiconnectedComponents>::const_iterator i = subObservations.begin(); i != subObservations.end(); i++)
					{
						distribution.add(i->getState());
					}
				}
				else
				{
					for(std::vector< ::residualConnectivity::obs::usingBiconnectedComponents>::const_iterator i = observations.begin(); i != observations.end(); i++)
					{
						distribution.add(i->getState());
					}

				}
				oarchive << distribution;
				stream.close();
			}
			else
			{
				args.outputStream << "Error writing to file " << outputDistributionFile << outputObject::endl;
			}
		}
		if(outputTree)
		{
			args.outputStream << "Beginning tree layout...." << outputObject::flush;
			bool success  = tree.layout();
			if(!success) args.outputStream << "Unable to lay out tree graph! " << outputObject::endl;
			else 
			{
				args.outputStream << "Done" << outputObject::endl;
				std::ofstream stream(outputTreeFile.c_str());
				if(stream.is_open())
				{
					boost::archive::binary_oarchive oarchive(stream);
					oarchive << tree;
					stream.close();
				}
				else
				{
					args.outputStream << "Error writing to file " << outputTreeFile << outputObject::endl;
				}
			}
		}
	}
}

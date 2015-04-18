#include <boost/archive/basic_archive.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningForSplitting.h"
#include "subObs/articulationConditioningForSplitting.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "bridges.hpp"
#include <fstream>
#include "obs/getSubObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
#include "argumentsMPFR.h"
namespace discreteGermGrain
{
	struct stepInputs
	{
		stepInputs(Context const& context, const std::vector<float>& splittingFactors)
			:context(context), splittingFactors(splittingFactors)
		{}
		Context const& context;
		const std::vector<float>& splittingFactors;
		int initialRadius;
		long n;
		bool outputTree;
	};
	struct stepOutputs
	{
		stepOutputs(std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting>& subObservations, std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting>& observations, boost::mt19937& randomSource, observationTree& tree)
			:subObservations(subObservations), observations(observations), randomSource(randomSource), tree(tree)
		{}
		std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting>& subObservations;
		std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting>& observations;
		std::vector<int> potentiallyConnectedIndices;
		boost::mt19937& randomSource;
		observationTree& tree;
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
			std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting> observationsThisThread;
			std::vector<int> parentIndicesThisThread;
			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(inputs.context.nVertices());
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
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
				int previousLength = observationsThisThread.size();
				//get out the child observations
				outputs.subObservations[j].estimateRadius1(
#ifdef USE_OPENMP
						perThreadSource
#else
						outputs.randomSource
#endif
						, nThisObservation, connectedComponents, stack, observationsThisThread);
				int newLength = observationsThisThread.size();
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
		std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting> nextSetObservations;
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
				std::vector<int> connectedComponents(inputs.context.nVertices());
				//stack for depth first search
				boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
				boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;
				//used to calculate the splitting factor (which is random)
				boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);

				::discreteGermGrain::subObs::withWeightConstructorType getSubObsHelper(connectedComponents, stack, subGraphStack);
				::discreteGermGrain::obs::withWeightConstructorType getObsHelper;
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
					const ::discreteGermGrain::subObs::articulationConditioningForSplitting& currentObs = outputs.subObservations[j];
					for(int k = 0; k < nThisObservation; k++)
					{
						::discreteGermGrain::obs::articulationConditioningForSplitting obs = ::discreteGermGrain::subObs::getObservation<::discreteGermGrain::subObs::articulationConditioningForSplitting>::get(currentObs, 
#ifdef USE_OPENMP
								perThreadSource
#else
								outputs.randomSource
#endif
						, getObsHelper);
						::discreteGermGrain::subObs::articulationConditioningForSplitting subObs = ::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::articulationConditioningForSplitting>::get(obs, inputs.initialRadius - i, getSubObsHelper);
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
			std::cout << "Finished splitting step " << i << " / " << inputs.initialRadius << ", " << nextSetObservations.size() << " / " << generated+1 << " observations continuing" << std::endl;
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
			std::vector<int> connectedComponents(inputs.context.nVertices());
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;
			::discreteGermGrain::subObs::withWeightConstructorType helper(connectedComponents, stack, subGraphStack);
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			#pragma omp for
#endif
			for(int i = 0; i < inputs.n; i++)
			{
				::discreteGermGrain::obs::articulationConditioningForSplitting obs(inputs.context, 
#ifdef USE_OPENMP
						perThreadSource
#else
						outputs.randomSource
#endif
						);
				::discreteGermGrain::subObs::articulationConditioningForSplitting subObs(::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::articulationConditioningForSplitting>::get(obs, inputs.initialRadius, helper));
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
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			INPUT_GRAPH_OPTION
			PROBABILITY_OPTION
			SEED_OPTION
			INITIAL_RADIUS_OPTION
			N_OPTION
			SPLITTING_FACTOR_OPTION
			OUTPUT_DISTRIBUTION_OPTION
			OUTPUT_TREE_OPTION
			HELP_OPTION;

		boost::program_options::variables_map variableMap;
		try
		{
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variableMap);
		}
		catch(boost::program_options::error& ee)
		{
			std::cerr << "Error parsing command line arguments: " << ee.what() << std::endl << std::endl;
			std::cerr << options << std::endl;
			return -1;
		}
		if(variableMap.count("help") > 0)
		{
			std::cout <<
				"This uses a splitting approach to estimate the probability that a random subgraph is connected. The random subgraph uses a vertex percolation model."
				"\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		std::string message;
		mpfr_class probability;
		if(!readProbabilityString(variableMap, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		Context context = Context::gridContext(1, probability);
		if(!readContext(variableMap, context, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		int initialRadius;
		if(!readInitialRadius(variableMap, initialRadius))
		{
			return 0;
		}


		int n;
		if(!readN(variableMap, n))
		{
			return 0;
		}
		//can input a single splitting factor, or one for each level
		if(variableMap.count("splittingFactor") != 1)
		{
			std::cout << "Please enter option splittingFactor once" << std::endl;
			return 0;
		}
		std::vector<float> splittingFactors = variableMap["splittingFactor"].as<std::vector<float> >();
		if(splittingFactors.size() == 1)
		{
			splittingFactors.insert(splittingFactors.end(), initialRadius - 1, splittingFactors[0]);
		}
		if((int)splittingFactors.size() != initialRadius)
		{
			std::cout << "Wrong number of values entered for input splittingFactor" << std::endl;
			return 0;
		}
		observationTree tree(&context, initialRadius);
		bool outputTree = variableMap.count("outputTree");


		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm
		std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting> subObservations;
		std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting> observations;

		stepInputs inputs(context, splittingFactors);
		inputs.initialRadius = initialRadius;
		inputs.outputTree = outputTree;
		inputs.n = n;
		stepOutputs outputs(subObservations, observations, randomSource, tree);
		outputs.totalGenerated = 0;

		doCrudeMCStep(inputs, outputs);
		std::cout << "Retaining " << subObservations.size() << " / " << n << " observations from crude MC step" << std::endl;		

		mpfr_class finalEstimate;
		if(initialRadius != 0)
		{
			//This does nothing for the case initialRadius = 1
			stepsExceptOne(inputs, outputs);
			
			//When the radius is 1 we use a different algorithm
			stepOne(inputs, outputs);
			std::cout << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << observations.size() << " / " << outputs.totalGenerated  << " observations had non-zero probability" << std::endl;

			mpfr_class probabilitySum = 0;
			for(std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting>::iterator i = observations.begin(); i != observations.end(); i++)
			{
				probabilitySum += i->getWeight();
			}
			mpfr_class averageLastStep = probabilitySum / outputs.totalGenerated;
			std::cout << "Average probability at last step was " << averageLastStep.str() << std::endl;
			mpfr_class totalSampleSize = n;
			for(std::vector<float>::iterator i = splittingFactors.begin(); i != splittingFactors.end(); i++) totalSampleSize *= *i;
			finalEstimate = probabilitySum / totalSampleSize;
			//Swap the vectors over, so that the results are always stored in finalObservations. Just in case we want to 
			//revert back to using the allExceptOne algorithm for all the steps. 
		}
		else
		{
			mpfr_class probabilitySum = 0;
			for(std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting>::iterator i = subObservations.begin(); i != subObservations.end(); i++)
			{
				probabilitySum += i->getWeight();
			}
			finalEstimate = probabilitySum / n;
		}


		if(variableMap.count("outputDistribution") > 0)
		{
			std::string file(variableMap["outputDistribution"].as<std::string>());
			std::ofstream stream(file.c_str());
			if(stream.is_open())
			{
				boost::archive::binary_oarchive oarchive(stream);
				empiricalDistribution distribution(true, context.nVertices(), context);
				if(initialRadius == 0)
				{
					for(std::vector<::discreteGermGrain::subObs::articulationConditioningForSplitting>::const_iterator i = subObservations.begin(); i != subObservations.end(); i++)
					{
						distribution.add(i->getState());
					}
				}
				else
				{
					for(std::vector<::discreteGermGrain::obs::articulationConditioningForSplitting>::const_iterator i = observations.begin(); i != observations.end(); i++)
					{
						distribution.add(i->getState());
					}

				}
				oarchive << distribution;
				stream.close();
			}
			else
			{
				std::cout << "Error writing to file " << variableMap["outputDistribution"].as<std::string>() << std::endl;
			}
		}
		if(outputTree)
		{
			std::cout << "Beginning tree layout....";
			std::cout.flush();
			bool success  = tree.layout();
			if(!success) std::cout << "Unable to lay out tree. Was graphviz support enabled? " << std::endl;
			else 
			{
				std::cout << "Done" << std::endl;
				std::string file(variableMap["outputTree"].as<std::string>());
				std::ofstream stream(file.c_str());
				if(stream.is_open())
				{
					boost::archive::binary_oarchive oarchive(stream);
					oarchive << tree;
					stream.close();
				}
				else
				{
					std::cout << "Error writing to file " << variableMap["outputTree"].as<std::string>() << std::endl;
				}
			}
		}
		std::cout << "Estimated probability was " << finalEstimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

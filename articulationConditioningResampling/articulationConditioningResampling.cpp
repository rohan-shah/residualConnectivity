#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningForResampling.h"
#include "subObs/articulationConditioningForResampling.h"
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
		stepInputs(Context const& context)
			:context(context)
		{}
		Context const& context;
		int initialRadius;
		long n;
		bool outputTree;
	};
	struct stepOutputs
	{
		stepOutputs(std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>& subObservations, std::vector<::discreteGermGrain::obs::articulationConditioningForResampling>& observations, boost::mt19937& randomSource, observationTree& tree)
			:subObservations(subObservations), observations(observations), randomSource(randomSource), tree(tree)
		{}
		std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>& subObservations;
		std::vector<::discreteGermGrain::obs::articulationConditioningForResampling>& observations;
		std::vector<int> potentiallyConnectedIndices;
		boost::mt19937& randomSource;
		observationTree& tree;
		std::vector<std::ptrdiff_t> aliasMethodTemporary1, aliasMethodTemporary2;
		std::vector<std::pair<double, std::ptrdiff_t> > aliasMethodTemporary3;
	};
	void stepsExceptFirst(stepInputs& inputs, stepOutputs& outputs)
	{
		std::vector<double> resamplingProbabilities;
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int j = 0; j < 100; j++) perThreadSeeds[j] = outputs.randomSource();
#endif
		std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling> nextSetObservations;
		std::vector<int> nextStepPotentiallyConnectedIndices;
		//Loop over the splitting steps (the different nested events)
		for(int i = 1; i < inputs.initialRadius+1; i++)
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
				std::vector<int> connectedComponents(inputs.context.nVertices());
				//stack for depth first search
				boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
				boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;

				::discreteGermGrain::subObs::articulationConditioningForResamplingConstructorType getSubObsHelper(connectedComponents, stack, subGraphStack);
				getSubObsHelper.useConditioning = true;
				::discreteGermGrain::obs::withWeightConstructorType getObsHelper;
#ifdef USE_OPENMP
				//per-thread random number generation
				boost::mt19937 perThreadSource;
				perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);

				#pragma omp for
#endif
				for(int j = 0; j < (int)outputs.subObservations.size(); j++)
				{
					//get out the current observation
					const ::discreteGermGrain::subObs::articulationConditioningForResampling& currentObs = outputs.subObservations[j];
					::discreteGermGrain::obs::articulationConditioningForResampling obs = ::discreteGermGrain::subObs::getObservation<::discreteGermGrain::subObs::articulationConditioningForResampling>::get(currentObs, 
#ifdef USE_OPENMP
							perThreadSource
#else
							outputs.randomSource
#endif
					, getObsHelper);
					::discreteGermGrain::subObs::articulationConditioningForResampling subObs = ::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::articulationConditioningForResampling>::get(obs, inputs.initialRadius - i, getSubObsHelper);
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
			std::cout << "Finished splitting step " << i << " / " << inputs.initialRadius << std::endl;
			outputs.subObservations.clear();
			outputs.potentiallyConnectedIndices.clear();
			mpfr_class sum = 0;
			resamplingProbabilities.clear();
			for(std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>::iterator j = nextSetObservations.begin(); j != nextSetObservations.end(); j++)
			{
				sum += j->getWeight();
				resamplingProbabilities.push_back(j->getWeight().convert_to<double>());
			}
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
			std::vector<int> connectedComponents(inputs.context.nVertices());
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType subGraphStack;
			::discreteGermGrain::subObs::articulationConditioningForResamplingConstructorType helper(connectedComponents, stack, subGraphStack);
			helper.useConditioning = false;
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			#pragma omp for
#endif
			for(int i = 0; i < inputs.n; i++)
			{
				::discreteGermGrain::obs::articulationConditioningForResampling obs(inputs.context, 
#ifdef USE_OPENMP
						perThreadSource
#else
						outputs.randomSource
#endif
						);
				::discreteGermGrain::subObs::articulationConditioningForResampling subObs(::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::articulationConditioningForResampling>::get(obs, inputs.initialRadius, helper));
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
		std::vector<double> resamplingProbabilities;
		mpfr_class sum = 0;
		for(std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>::iterator i = outputs.subObservations.begin(); i != outputs.subObservations.end(); i++)
		{
			sum += i->getWeight();
			resamplingProbabilities.push_back(i->getWeight().convert_to<double>());
		}
		mpfr_class averageWeight = sum / inputs.n;
		aliasMethod::aliasMethod alias(resamplingProbabilities, sum.convert_to<double>(), outputs.aliasMethodTemporary1, outputs.aliasMethodTemporary2, outputs.aliasMethodTemporary3);
		std::vector<int> nextPotentiallyConnectedIndices;
		std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling> nextSubObservations;
		for(int i = 0; i < inputs.n; i++)
		{
			int index = (int)alias(outputs.randomSource);
			nextSubObservations.push_back(outputs.subObservations[index].copyWithWeight(averageWeight));
			nextPotentiallyConnectedIndices.push_back(outputs.potentiallyConnectedIndices[index]);
		}
		outputs.subObservations.swap(nextSubObservations);
		outputs.potentiallyConnectedIndices.swap(nextPotentiallyConnectedIndices);
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
				"This uses a resampling approach to estimate the probability that a random subgraph is connected. The random subgraph uses a vertex percolation model."
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
		observationTree tree(&context, initialRadius);
		bool outputTree = variableMap.count("outputTree");


		std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling> subObservations;
		std::vector<::discreteGermGrain::obs::articulationConditioningForResampling> observations;

		stepInputs inputs(context);
		inputs.initialRadius = initialRadius;
		inputs.outputTree = outputTree;
		inputs.n = n;
		stepOutputs outputs(subObservations, observations, randomSource, tree);

		doCrudeMCStep(inputs, outputs);

		mpfr_class finalEstimate;
		stepsExceptFirst(inputs, outputs);

		mpfr_class probabilitySum = 0;
		for(std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>::iterator i = subObservations.begin(); i != subObservations.end(); i++)
		{
			probabilitySum += i->getWeight();
		}
		finalEstimate = probabilitySum / n;


/*		if(variableMap.count("outputDistribution") > 0)
		{
			std::string file(variableMap["outputDistribution"].as<std::string>());
			std::ofstream stream(file.c_str());
			if(stream.is_open())
			{
				boost::archive::binary_oarchive oarchive(stream);
				empiricalDistribution distribution(true, context.nVertices(), context);
				if(initialRadius == 0)
				{
					for(std::vector<::discreteGermGrain::subObs::articulationConditioningForResampling>::const_iterator i = subObservations.begin(); i != subObservations.end(); i++)
					{
						distribution.add(i->getState());
					}
				}
				else
				{
					for(std::vector<::discreteGermGrain::obs::articulationConditioningForResampling>::const_iterator i = observations.begin(); i != observations.end(); i++)
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
		}*/
		std::cout << "Estimated probability was " << finalEstimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

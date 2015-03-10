#include <boost/archive/basic_archive.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include <boost/random/mersenne_twister.hpp>
#include "DiscreteGermGrainObs.h"
#include "DiscreteGermGrainSubObs.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/graphml.hpp>
#include "bridges.hpp"
#include <fstream>
#include <omp.h>
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
#include "argumentsMPIR.h"
namespace discreteGermGrain
{
	void stepOne(const std::vector<DiscreteGermGrainObs>& observations, std::vector<DiscreteGermGrainObs>& finalObservations, std::vector<float>& probabilities, boost::mt19937& randomSource, int initialRadius, Context const& context, const std::vector<float>& splittingFactors)
	{
		boost::mt19937::result_type perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = randomSource();

		float sptittingFactorRemainder = splittingFactors[initialRadius-1] - floor(splittingFactors[initialRadius-1]);
		int splittingFactorInteger = (int)floor(splittingFactors[initialRadius-1]);

		//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
#ifdef USE_OPENMP
		#pragma omp parallel
#endif
		{
			//vector of final observations from this thread. Set up this way to keep things deterministic
			std::vector<DiscreteGermGrainObs> finalObservationsThisThread;
			std::vector<float> probabilitiesThisThread;

			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(boost::num_vertices(context.getGraph()));
			//used to calculate the splitting factor (which is random)
			boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);
				
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
#ifdef USE_OPENMP
			#pragma omp for
#endif
			for(int j = 0; j < (int)observations.size(); j++)
			{
				//number of observations which the current observation is split into
				int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
				//get out the current observation
				const DiscreteGermGrainObs& currentObs = observations[j];
				DiscreteGermGrainSubObs subObs = currentObs.getSubObservation(1);
				float probability;
				boost::shared_array<const vertexState> newState = subObs.estimateRadius1(perThreadSource, nThisObservation, probability);
				probabilitiesThisThread.push_back(probability);
				if(probability > 0)
				{
					DiscreteGermGrainObs finalObs(context, newState);
					finalObservationsThisThread.push_back(std::move(finalObs));
				}
			}
			for(int j = 0; j < omp_get_num_threads(); j++)
			{
#ifdef USE_OPENMP
				#pragma omp barrier 
				#pragma omp critical
#endif
				{
					if(j == omp_get_thread_num())
					{
						finalObservations.insert(finalObservations.end(), std::make_move_iterator(finalObservationsThisThread.begin()), std::make_move_iterator(finalObservationsThisThread.end()));
						probabilities.insert(probabilities.end(), std::make_move_iterator(probabilitiesThisThread.begin()), std::make_move_iterator(probabilitiesThisThread.end()));
					}
				}
			}
		}
	}
	void stepsExceptOne(std::vector<float>& logRetainedProportion, std::vector<DiscreteGermGrainObs>& observations, boost::mt19937& randomSource, int initialRadius, Context const& context, const std::vector<float>& splittingFactors)
	{
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];

		//Loop over the splitting steps (the different nested events)
		for(int i = 1; i < initialRadius/*+1*/; i++)
		{
			float sptittingFactorRemainder = splittingFactors[i-1] - floor(splittingFactors[i-1]);
			int splittingFactorInteger = (int)floor(splittingFactors[i-1]);

			//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
			int generated = 0;
			std::vector<DiscreteGermGrainObs> nextSetObservations;
			
			//refresh per-thread random number seeds
			for(int j = 0; j < 100; j++) perThreadSeeds[j] = randomSource();
			//loop over the various sample paths
#ifdef USE_OPENMP
			#pragma omp parallel
#endif
			{
				//next set of observations from the current thread. Set up like this to make everything deterministic
				std::vector<DiscreteGermGrainObs> nextSetObservationsThisThread;

				//vector that we re-use to avoid allocations
				std::vector<int> connectedComponents(boost::num_vertices(context.getGraph()));
				//used to calculate the splitting factor (which is random)
				boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);
				
				//per-thread random number generation
				boost::mt19937 perThreadSource;
				perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);

#ifdef USE_OPENMP
				#pragma omp for
#endif
				for(int j = 0; j < (int)observations.size(); j++)
				{
					//number of observations which the current observation is split into
					int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
#ifdef USE_OPENMP
					#pragma omp atomic
#endif
					generated += nThisObservation;
					//get out the current observation
					const DiscreteGermGrainObs& currentObs = observations[j];
					DiscreteGermGrainSubObs subObs = currentObs.getSubObservation(initialRadius - i + 1);
					for(int k = 0; k < nThisObservation; k++)
					{
						bool singleComponent;
						DiscreteGermGrainObs newObs = subObs.getSingleComponentObservation(initialRadius - i, perThreadSource, connectedComponents, singleComponent);
						if(singleComponent)
						{
							nextSetObservationsThisThread.push_back(std::move(newObs));
						}
					}
				}
				for(int j = 0; j < omp_get_num_threads(); j++)
				{
#ifdef USE_OPENMP
					#pragma omp barrier 
					#pragma omp critical
#endif
					{
						if(j == omp_get_thread_num()) 
						{
							nextSetObservations.insert(nextSetObservations.end(), std::make_move_iterator(nextSetObservationsThisThread.begin()), std::make_move_iterator(nextSetObservationsThisThread.end()));
						}
					}
				}
			}
			std::cout << "Finished splitting step " << i << " / " << initialRadius << ", " << nextSetObservations.size() << " / " << generated << " observations continuing" << std::endl;
			logRetainedProportion.push_back(log((float)nextSetObservations.size() / (float)generated));
			observations.swap(nextSetObservations);
		}
	}
	void doCrudeMCStep(std::vector<DiscreteGermGrainObs>& observations, boost::mt19937& randomSource, Context const& context, int initialRadius, int n)
	{
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = randomSource();
#ifdef USE_OPENMP
		#pragma omp parallel
#endif
		{
			std::vector<int> scratchMemory;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			std::vector<DiscreteGermGrainObs> observationsThisThread;
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
#ifdef USE_OPENMP
			#pragma omp for
#endif
			for(int i = 0; i < n; i++)
			{
				DiscreteGermGrainObs obs(context, perThreadSource);
				DiscreteGermGrainSubObs subObs(obs.getSubObservation(initialRadius));
				if(isSingleComponentPossible(context, subObs.getState(), scratchMemory, stack))
				{
					//We need to get out an observation that has the subpattern points marked - The observation is just a pattern of 0's and 1's at this point, no fixed points
					bool successful;
					obs = subObs.getSingleComponentObservation(initialRadius, perThreadSource, scratchMemory, successful);
					if(!successful) 
					{
						throw std::runtime_error("Internal error");
					}
					observationsThisThread.push_back(std::move(obs));
				}
			}
			for(int j = 0; j < omp_get_num_threads(); j++)
			{
#ifdef USE_OPENMP
				#pragma omp barrier 
				#pragma omp critical
#endif
				{
					if(omp_get_thread_num() == j) observations.insert(observations.end(), std::make_move_iterator(observationsThisThread.begin()), std::make_move_iterator(observationsThisThread.end()));
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
			OUTPUT_FILE_OPTION
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


		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm
		//This means that logProbabilities may or may not contain any data. 

		std::vector<DiscreteGermGrainObs> observations;
		doCrudeMCStep(observations, randomSource, context, initialRadius, n);
		float crudeMCProbability = ((float)observations.size()) / n;
		std::cout << "Retaining " << observations.size() << " / " << n << " observations from crude MC step" << std::endl;		

		float finalEstimate;
		if(initialRadius != 0)
		{
			std::vector<float> logRetainedProportion;
			//This does nothing for the case initialRadius = 1
			stepsExceptOne(logRetainedProportion, observations, randomSource, initialRadius, context, splittingFactors);
			
			
			//When the radius is 1 we use a different algorithm
			std::vector<DiscreteGermGrainObs> finalObservations;
			std::vector<float> probabilities;
			stepOne(observations, finalObservations, probabilities, randomSource, initialRadius, context, splittingFactors);
			std::cout << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << finalObservations.size() << " / " << observations.size() << " observations had non-zero probability" << std::endl;

			float averageLastStep;
			{
				boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::sum_kahan> > acc;
				std::for_each(probabilities.begin(), probabilities.end(), std::ref(acc));
				averageLastStep = boost::accumulators::sum_kahan(acc) / probabilities.size();
			}
			std::cout << "Average probability at last step was " << averageLastStep << std::endl;
			float productRetainedProportion;
			{
				boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::sum_kahan> > acc;
				std::for_each(logRetainedProportion.begin(), logRetainedProportion.end(), std::ref(acc));
				productRetainedProportion = exp(boost::accumulators::sum_kahan(acc));
			}

			finalEstimate = crudeMCProbability * productRetainedProportion;
			finalEstimate *= averageLastStep;
			//Swap the vectors over, so that the results are always stored in finalObservations. Just in case we want to 
			//revert back to using the allExceptOne algorithm for all the steps. 
			observations.swap(finalObservations);
		}
		else
		{
			finalEstimate = crudeMCProbability;
		}


		if(variableMap.count("outputFile") > 0)
		{
			std::string file(variableMap["outputFile"].as<std::string>());
			std::ofstream stream(file.c_str());
			if(stream.is_open())
			{
				std::cout << "Writing to file..." << std::endl;
				boost::archive::text_oarchive oarchive(stream);
				for(std::vector<DiscreteGermGrainObs>::const_iterator i = observations.begin(); i != observations.end(); i++)
				{
					oarchive << *i;
				}
				stream.close();
			}
			else
			{
				std::cout << "Error writing to file..." << std::endl;
			}
		}
		//Take the sum of all the log probabilities, and then take the exponential.
		std::cout << "Estimated probability was " << finalEstimate << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

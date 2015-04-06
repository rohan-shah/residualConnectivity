#include <boost/archive/basic_archive.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include "empiricalDistribution.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/usingBiconnectedComponents.h"
#include "subObs/usingBiconnectedComponents.h"
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
	void stepOne(const std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents>& observations, std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents>& finalObservations, mpfr_class& probabilitySum, boost::mt19937& randomSource, int initialRadius, Context const& context, const std::vector<float>& splittingFactors)
	{
		probabilitySum = 0;
#ifdef USE_OPENMP
		boost::mt19937::result_type perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = randomSource();
#endif

		float sptittingFactorRemainder = splittingFactors[initialRadius-1] - floor(splittingFactors[initialRadius-1]);
		int splittingFactorInteger = (int)floor(splittingFactors[initialRadius-1]);

		//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
#ifdef USE_OPENMP
		#pragma omp parallel
#endif
		{
			//vector of final observations from this thread. Set up this way to keep things deterministic
			std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> finalObservationsThisThread;
			mpfr_class probabilitySumThisThread = 0;

			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(context.nVertices());
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType helper(connectedComponents, stack);
			//used to calculate the splitting factor (which is random)
			boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);
			
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			#pragma omp for
#endif
			for(int j = 0; j < (int)observations.size(); j++)
			{
				//number of observations which the current observation is split into
				int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(
#ifdef USE_OPENMP
						perThreadSource
#else
						randomSource
#endif
						);
				//get out the current observation
				mpfr_class probability;
				boost::shared_array<const vertexState> newState = observations[j].estimateRadius1(
#ifdef USE_OPENMP
						perThreadSource
#else
						randomSource
#endif
						, nThisObservation, connectedComponents, stack, probability);
				probabilitySumThisThread += probability;
				if(probability > 0)
				{
					::discreteGermGrain::subObs::usingBiconnectedComponents finalObs(context, newState, 0, helper);
					finalObservationsThisThread.push_back(std::move(finalObs));
				}
			}
#ifdef USE_OPENMP
			for(int j = 0; j < omp_get_num_threads(); j++)
			{
				#pragma omp barrier 
				#pragma omp critical
				{
					if(j == omp_get_thread_num())
					{
						finalObservations.insert(finalObservations.end(), std::make_move_iterator(finalObservationsThisThread.begin()), std::make_move_iterator(finalObservationsThisThread.end()));
						probabilitySum += probabilitySumThisThread;
					}
				}
			}
#else
			probabilitySum = probabilitySumThisThread;
			finalObservations.swap(finalObservationsThisThread);
#endif
		}
	}
	void stepsExceptOne(std::vector<mpfr_class>& retainedProportion, std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents>& observations, boost::mt19937& randomSource, int initialRadius, Context const& context, const std::vector<float>& splittingFactors)
	{
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int j = 0; j < 100; j++) perThreadSeeds[j] = randomSource();
#endif

		//Loop over the splitting steps (the different nested events)
		for(int i = 1; i < initialRadius/*+1*/; i++)
		{
			float sptittingFactorRemainder = splittingFactors[i-1] - floor(splittingFactors[i-1]);
			int splittingFactorInteger = (int)floor(splittingFactors[i-1]);

			//The number of sub-observations that were generated - If the splitting factor is not an integer, this will be random so we need to keep track of it. 
			int generated = 0;
			std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> nextSetObservations;
			
			//loop over the various sample paths
#ifdef USE_OPENMP
			#pragma omp parallel
#endif
			{
				//next set of observations from the current thread. Set up like this to make everything deterministic
				std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> nextSetObservationsThisThread;

				//vector that we re-use to avoid allocations
				std::vector<int> connectedComponents(context.nVertices());
				//stack for depth first search
				boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
				::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType helper(connectedComponents, stack);
				//used to calculate the splitting factor (which is random)
				boost::random::bernoulli_distribution<float> splittingFactorBernoulli(sptittingFactorRemainder);
		
#ifdef USE_OPENMP
				//per-thread random number generation
				boost::mt19937 perThreadSource;
				perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);

				#pragma omp for
#endif
				for(int j = 0; j < (int)observations.size(); j++)
				{
					//number of observations which the current observation is split into
#ifdef USE_OPENMP
					int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(perThreadSource);
					#pragma omp atomic
#else
					int nThisObservation = splittingFactorInteger + splittingFactorBernoulli(randomSource);
#endif
					generated += nThisObservation;
					//get out the current observation
					const ::discreteGermGrain::subObs::usingBiconnectedComponents& currentObs = observations[j];
					for(int k = 0; k < nThisObservation; k++)
					{
						::discreteGermGrain::obs::usingBiconnectedComponents obs = ::discreteGermGrain::subObs::getObservation<::discreteGermGrain::subObs::usingBiconnectedComponents>::get(currentObs, 
#ifdef USE_OPENMP
								perThreadSource
#else
								randomSource
#endif
						);
						::discreteGermGrain::subObs::usingBiconnectedComponents subObs = ::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::usingBiconnectedComponents>::get(obs, initialRadius - i, helper);
						if(subObs.isPotentiallyConnected())
						{
							nextSetObservationsThisThread.push_back(std::move(subObs));
						}
					}
				}
#ifdef USE_OPENMP
				for(int j = 0; j < omp_get_num_threads(); j++)
				{
					#pragma omp barrier 
					#pragma omp critical
					{
						if(j == omp_get_thread_num()) 
						{
							nextSetObservations.insert(nextSetObservations.end(), std::make_move_iterator(nextSetObservationsThisThread.begin()), std::make_move_iterator(nextSetObservationsThisThread.end()));
						}
					}
				}
#else
				nextSetObservations.swap(nextSetObservationsThisThread);
#endif
			}
			std::cout << "Finished splitting step " << i << " / " << initialRadius << ", " << nextSetObservations.size() << " / " << generated << " observations continuing" << std::endl;
			retainedProportion.push_back((mpfr_class)nextSetObservations.size() / (mpfr_class)generated);
			observations.swap(nextSetObservations);
		}
	}
	void doCrudeMCStep(std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents>& observations, boost::mt19937& randomSource, Context const& context, int initialRadius, int n)
	{
#ifdef USE_OPENMP
		//set up per-thread random number generators
		boost::mt19937::result_type perThreadSeeds[100];
		for(int i = 0; i < 100; i++) perThreadSeeds[i] = randomSource();
		#pragma omp parallel
#endif
		{
			//vector that we re-use to avoid allocations
			std::vector<int> connectedComponents(context.nVertices());
			//stack for depth first search
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType helper(connectedComponents, stack);
			std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> observationsThisThread;
#ifdef USE_OPENMP
			//per-thread random number generation
			boost::mt19937 perThreadSource;
			perThreadSource.seed(perThreadSeeds[omp_get_thread_num()]);
			#pragma omp for
#endif
			for(int i = 0; i < n; i++)
			{
				::discreteGermGrain::obs::usingBiconnectedComponents obs(context, 
#ifdef USE_OPENMP
						perThreadSource
#else
						randomSource
#endif
						);
				::discreteGermGrain::subObs::usingBiconnectedComponents subObs(::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::usingBiconnectedComponents>::get(obs, initialRadius, helper));
				if(subObs.isPotentiallyConnected())
				{
					observationsThisThread.push_back(std::move(subObs));
				}
			}
#ifdef USE_OPENMP
			for(int j = 0; j < omp_get_num_threads(); j++)
			{
				#pragma omp barrier 
				#pragma omp critical
				{
					if(omp_get_thread_num() == j) observations.insert(observations.end(), std::make_move_iterator(observationsThisThread.begin()), std::make_move_iterator(observationsThisThread.end()));
				}
			}
#else
			observations.swap(observationsThisThread);
#endif
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


		//There are three distinct cases here.
		//1. initialRadius = 0, only crude MC step
		//2. initialRadius = 1, only the crude MC and then one type of algorithm
		//3. initialRadius >= 2, crude MC and then two types of algorithm

		std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> observations;
		doCrudeMCStep(observations, randomSource, context, initialRadius, n);
		float crudeMCProbability = ((float)observations.size()) / n;
		std::cout << "Retaining " << observations.size() << " / " << n << " observations from crude MC step" << std::endl;		

		mpfr_class finalEstimate;
		if(initialRadius != 0)
		{
			std::vector<mpfr_class> retainedProportion;
			//This does nothing for the case initialRadius = 1
			stepsExceptOne(retainedProportion, observations, randomSource, initialRadius, context, splittingFactors);
			
			
			//When the radius is 1 we use a different algorithm
			std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents> finalObservations;
			mpfr_class probabilitySum = 0;
			int lastStepObservations = observations.size();
			stepOne(observations, finalObservations, probabilitySum, randomSource, initialRadius, context, splittingFactors);
			std::cout << "Finished splitting step " << initialRadius << " / " << initialRadius << ", " << finalObservations.size() << " / " << observations.size() << " observations had non-zero probability" << std::endl;

			mpfr_class averageLastStep = probabilitySum / lastStepObservations;
			std::cout << "Average probability at last step was " << averageLastStep.str() << std::endl;
			mpfr_class productRetainedProportion = 1;
			for(std::vector<mpfr_class>::iterator i = retainedProportion.begin(); i != retainedProportion.end(); i++) productRetainedProportion *= *i;
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


		if(variableMap.count("outputDistribution") > 0)
		{
			std::string file(variableMap["outputDistribution"].as<std::string>());
			std::ofstream stream(file.c_str());
			if(stream.is_open())
			{
				boost::archive::binary_oarchive oarchive(stream);
				empiricalDistribution distribution(true, context.nVertices(), context);
				/*for(std::vector<::discreteGermGrain::subObs::usingBiconnectedComponents>::const_iterator i = observations.begin(); i != observations.end(); i++)
				{
					distribution.add(i->getState(), i->);
				}*/
				oarchive << distribution;
				stream.close();
			}
			else
			{
				std::cout << "Error writing to file " << variableMap["outputDistribution"].as<std::string>() << std::endl;
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

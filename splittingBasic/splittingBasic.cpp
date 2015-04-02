#include <boost/archive/basic_archive.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include <boost/random/mersenne_twister.hpp>
#include "DiscreteGermGrainObs.h"
#include "DiscreteGermGrainSubObs.h"
#include "obs/basic.h"
#include "subObs/basic.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include "bridges.hpp"
#include <fstream>
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
#include "argumentsMPFR.h"
namespace discreteGermGrain
{
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

		std::vector<::discreteGermGrain::subObs::basic> observations, nextStepObservations;
		//vector that we re-use to avoid allocations
		std::vector<int> connectedComponents(context.nVertices());
		std::vector<unsigned long long> retained(initialRadius+1, 0);
		std::vector<unsigned long long> total(initialRadius+1, 0);
		//stack for depth first search
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		::discreteGermGrain::subObs::basicConstructorType helper(connectedComponents, stack);

		std::vector<boost::random::bernoulli_distribution<float> > bernoullis;
		for(std::vector<float>::iterator j = splittingFactors.begin(); j != splittingFactors.end(); j++)
		{
			bernoullis.push_back(boost::random::bernoulli_distribution<float>(*j - floor(*j)));
		}
		for(int i = 0; i < n; i++)
		{
			observations.clear();
			nextStepObservations.clear();

			::discreteGermGrain::obs::basic obs(context, randomSource);
			::discreteGermGrain::subObs::basic subObs(::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::basic>::get(obs, initialRadius, helper));
			total[0]++;
			if(subObs.isPotentiallyConnected())
			{
				retained[0]++;
				observations.push_back(std::move(subObs));
			}
			for(int radius = initialRadius - 1; radius >= 0; radius--)
			{
				nextStepObservations.clear();
				float splittingFactor = splittingFactors[initialRadius-radius-1];
				for(std::vector<::discreteGermGrain::subObs::basic>::iterator subObsIterator = observations.begin(); subObsIterator != observations.end(); subObsIterator++)
				{
					int integerSplittingFactor = splittingFactor;
					if(bernoullis[initialRadius-radius-1](randomSource)) integerSplittingFactor++;
					for(int k = 0; k < integerSplittingFactor; k++)
					{
						::discreteGermGrain::obs::basic newObs = ::discreteGermGrain::subObs::getObservation<::discreteGermGrain::subObs::basic>::get(*subObsIterator, randomSource);
						::discreteGermGrain::subObs::basic newSubObs(::discreteGermGrain::obs::getSubObservation<::discreteGermGrain::obs::basic>::get(newObs, radius, helper));
						total[initialRadius-radius]++;
						if(newSubObs.isPotentiallyConnected())
						{
							retained[initialRadius-radius]++;
							nextStepObservations.push_back(std::move(newSubObs));
						}
					}
				}
				nextStepObservations.swap(observations);
			}
		}

		mpfr_class totalSamples = n;
		for(std::vector<float>::iterator i = splittingFactors.begin(); i != splittingFactors.end(); i++)
		{
			totalSamples *= *i;
		}
		mpfr_class finalEstimate = *retained.rbegin() / totalSamples;
		std::cout << "Retaining " << retained[0] << " / " << n << " observations from crude MC step" << std::endl;
		for(int i = 1; i < initialRadius+1; i++)
		{
			std::cout << "Finished splitting step " << i << " / " << initialRadius << ", " << retained[i] << " / " << total[i] << " observations continuing" << std::endl;		
		}
		if(variableMap.count("outputFile") > 0)
		{
			std::string file(variableMap["outputFile"].as<std::string>());
			std::ofstream stream(file.c_str());
			if(stream.is_open())
			{
				std::cout << "Writing to file..." << std::endl;
				boost::archive::text_oarchive oarchive(stream);
				for(std::vector<::discreteGermGrain::subObs::basic>::const_iterator i = observations.begin(); i != observations.end(); i++)
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
		std::cout << "Estimated probability was " << finalEstimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

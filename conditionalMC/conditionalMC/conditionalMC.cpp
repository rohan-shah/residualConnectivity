#include <iostream>
#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "observation.h"
#include "Context.h"
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
#include "argumentsMPFR.h"
#include "subObs/subObs.h"
#include "conditionalMCLib.h"
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			INPUT_GRAPH_OPTION
			PROBABILITY_OPTION
			N_OPTION
			SEED_OPTION
			EXPECTED_UP_NUMBER_OPTION
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
				"This program estimates the probability that a random subgraph of an n * n grid graph is connected. The random subgraph uses a vertex percolation model.\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		int n;
		if(!readN(variableMap, n))
		{
			return 0;
		}

		mpfr_class probability;
		std::string message;
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
		bool outputExpectedUpNumber = variableMap["expectedUpNumber"].as<bool>();
		
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		conditionalMCArgs args(context, randomSource);
		args.n = n;
		args.probability = probability;
		conditionalMC(args);

		std::cout << "Connectivity estimate is " << args.estimate.convert_to<double>() << std::endl;
		if(outputExpectedUpNumber)
		{
			std::cout << "Expected number of up vertices conditional on having a connected graph was " << args.expectedUpNumber.convert_to<double>() << std::endl;
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

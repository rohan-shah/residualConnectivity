#include <boost/program_options.hpp>
#include <iostream>
#include "usingBiconnectedSplittingLib.h"
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

		commandLineOutput outputStream;
		usingBiconnectedSplittingArgs args(context, tree, outputStream);
		args.outputTree = variableMap.count("outputTree");
		args.outputTreeFile = variableMap["outputTree"].as<std::string>();
		args.n = n;
		args.splittingFactors = splittingFactors;
		args.initialRadius = initialRadius;
		args.estimate = 0;
		args.outputDistribution = variableMap.count("outputDistribution");
		args.outputDistributionFile = variableMap["outputDistribution"].as<std::string>();

		readSeed(variableMap, args.randomSource);
		usingBiconnectedSplitting(args);
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

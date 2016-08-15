#include <boost/archive/basic_archive.hpp>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include "arguments.h"
#include "argumentsMPFR.h"
#include "monteCarloMethods/articulationConditioningResampling.h"
#include "outputObject.h"
namespace residualConnectivity
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
			OUTPUT_DISTRIBUTION_OPTION
			OUTPUT_TREE_OPTION
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
				"This uses a resampling approach to estimate the probability that a random subgraph is connected. The random subgraph uses a vertex percolation model."
				"\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		std::string message;
		std::vector<mpfr_class> probabilities;
		if(!readProbabilityString(variableMap, probabilities, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		boost::shared_ptr<context::inputGraph> graph(new context::inputGraph());
		boost::shared_ptr<std::vector<context::vertexPosition> > vertexPositions(new std::vector<context::vertexPosition>());
		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>());
		bool successful = readGraph(variableMap, *graph.get(), *vertexPositions.get(), *ordering.get(), message);
		if(!successful)
		{
			std::cout << message << std::endl;
			return 0;
		}
		context contextObj(graph, ordering, vertexPositions, probabilities);

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

		bool outputExpectedUpNumber = variableMap["expectedUpNumber"].as<bool>();
		bool outputTree = variableMap.count("outputTree");
		observationTree tree(&contextObj, initialRadius);
		commandLineOutput output;

		articulationConditioningResamplingArgs args(contextObj, randomSource, tree, output);
		args.n = n;
		args.initialRadius = initialRadius;
		args.outputTree = outputTree;

		articulationConditioningResampling(args);
		std::cout << "Estimated probability was " << args.estimate.str() << std::endl;
		if(outputExpectedUpNumber)
		{
			std::cout << "Expected number of up vertices conditional on having a connected graph was " << args.expectedUpNumber.convert_to<double>() << std::endl;
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}

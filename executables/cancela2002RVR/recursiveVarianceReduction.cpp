#include "context.h"
#include "arguments.h"
#include "argumentsMPFR.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/program_options.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/graph/connected_components.hpp>
#include "connected_components_restricted.hpp"
#include "monteCarloMethods/recursiveVarianceReduction.h"
namespace residualConnectivity
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			INPUT_GRAPH_OPTION
			PROBABILITY_OPTION
			SEED_OPTION
			N_OPTION
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
		mpfr_class opProbability, inopProbability;
		if(!readProbabilityString(variableMap, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		context contextObj = context::gridContext(1, opProbability);
		if(!readContext(variableMap, contextObj, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		std::size_t nVertices = contextObj.nVertices();
		if(nVertices <= 1)
		{
			std::cout << "Estimate is 1" << std::endl;
			return 0;
		}
		boost::mt19937 randomSource;
		if(variableMap.count("seed") > 0)
		{
			randomSource.seed(variableMap["seed"].as<int>());
		}
	
		int n;
		if(!readN(variableMap, n))
		{
			std::cout << "Error reading input `n'" << std::endl;
			return 0;
		}
		recursiveVarianceReductionArgs args(contextObj, randomSource);
		args.n = n;
		recursiveVarianceReduction(args);
		std::cout << "Reliability estimate was " << args.estimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}

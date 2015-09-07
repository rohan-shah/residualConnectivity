#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "monteCarloMethods/crudeMC.h"
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
			N_OPTION
			SEED_OPTION
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

		std::string message;
		mpfr_class opProbability;
		if(!readProbabilityString(variableMap, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		Context context = Context::gridContext(1, opProbability);
		if(!readContext(variableMap, context, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);
		crudeMCArgs args(context, randomSource);
		args.n = n;

		std::size_t connected = crudeMC(args);
		std::cout << connected << " / " << n << " = " << ((float)connected / (float)n) << " had one connected component" << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

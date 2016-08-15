#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "monteCarloMethods/crudeMC.h"
#include "arguments.h"
#include "argumentsMPFR.h"
namespace residualConnectivity
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
		if (!successful)
		{
			std::cout << message << std::endl;
			return 0;
		}
		context contextObj(graph, ordering, vertexPositions, probabilities);
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);
		crudeMCArgs args(contextObj, randomSource);
		args.n = n;

		std::size_t connected = crudeMC(args);
		std::cout << connected << " / " << n << " = " << ((float)connected / (float)n) << " had one connected component" << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}

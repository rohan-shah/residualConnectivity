#include <boost/program_options.hpp>
#include "stochasticEnumerationLib.h"
#include "arguments.h"
#include "argumentsMPFR.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			INPUT_GRAPH_OPTION
			N_OPTION
			SEED_OPTION
			HELP_OPTION
			("vertexCount", boost::program_options::value<int>(), "(int) Number of UP vertices")
			;

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
				"This program estimates the probability that a random subgraph of a specified base graph is connected, using Stochastic Enumeration. The random subgraph uses a vertex percolation model.\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		int n;
		if(!readN(variableMap, n))
		{
			return 0;
		}
		if(n <= 2)
		{
			std::cout << "Input n must be at least 2" << std::endl;
			return 0;
		}

		std::string message;
		mpfr_class opProbability;
		if(!readProbabilityString(variableMap, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		mpfr_class inopProbability = 1 - opProbability;
		Context context = Context::gridContext(1, opProbability);
		if(!readContext(variableMap, context, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		const Context::inputGraph& graph = context.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);
		boost::random_number_generator<boost::mt19937> generator(randomSource);

		stochasticEnumerationArgs args(graph, randomSource);
		args.n = n;
		//Do we want whole spectra, or just a component?
		if (variableMap.count("vertexCount") < 1)
		{
			
		}
		else
		{
			int vertexCount = variableMap["vertexCount"].as<int>();
			if (vertexCount < 0 || vertexCount > nVertices)
			{
				std::cout << "Input vertexCount must be a value in [0, nVertices]" << std::endl;
				return 0;
			}
			args.vertexCount = vertexCount;

			bool result = stochasticEnumeration(args);
			if (result)
			{
				std::cout << "Estimated spectra is " << args.estimate.str() << std::endl;
			}
			else
			{
				std::cout << "Error: " << args.message << std::endl;
			}
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

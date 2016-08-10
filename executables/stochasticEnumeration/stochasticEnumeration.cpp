#include <boost/program_options.hpp>
#include "monteCarloMethods/stochasticEnumeration.h"
#include "arguments.h"
#include "argumentsMPFR.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
namespace residualConnectivity
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
			("optimized", boost::program_options::bool_switch()->default_value(true)->implicit_value(true))
			("nPermutations", boost::program_options::value<int>()->default_value(1)->implicit_value(1), "(int) Number of permutations")
			("outputStatus", boost::program_options::bool_switch()->default_value(false)->implicit_value(true), "Should we display progress?")
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

		int nPermutations = variableMap["nPermutations"].as<int>();
		if (nPermutations < 1)
		{
			std::cout << "Input nPermutations must be at least 1" << std::endl;
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
		context contextObj = context::gridContext(1, opProbability);
		if(!readContext(variableMap, contextObj, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		const context::inputGraph& graph = contextObj.getGraph();
		const std::size_t nVertices = boost::num_vertices(graph);
		boost::random_number_generator<boost::mt19937> generator(randomSource);
		bool optimized = variableMap["optimized"].as<bool>();

		stochasticEnumerationArgs args(graph, randomSource);
		args.n = n;
		args.nPermutations = nPermutations;
		bool outputStatus = variableMap["outputStatus"].as<bool>();
		//Do we want whole spectra, or just a component?
		if (variableMap.count("vertexCount") < 1)
		{
			std::vector<mpfr_class> estimates;
			estimates.reserve(nVertices + 1);
			for (int i = 0; i < (int)nVertices+1; i++)
			{
				args.vertexCount = i;
				bool result;
				if(optimized) result = stochasticEnumeration2(args);
				else result = stochasticEnumeration1(args);
				if (!result)
				{
					std::cout << "Error: " << args.message << std::endl;
					return 0;
				}
				estimates.push_back(args.estimate);
				if(outputStatus)
				{
					std::cout << "Finished couting subgraphs of size " << i << " / " << nVertices << std::endl;
				}
			}
			std::cout << "Estimated spectra is " << std::endl;
			for (int i = 0; i < (int)nVertices + 1; i++)
			{
				std::cout << "[" << i << "]: " << estimates[i].str() << std::endl;
			}

		}
		else
		{
			int vertexCount = variableMap["vertexCount"].as<int>();
			if (vertexCount < 0 || vertexCount > (int)nVertices)
			{
				std::cout << "Input vertexCount must be a value in [0, nVertices]" << std::endl;
				return 0;
			}
			args.vertexCount = vertexCount;

			bool result;
			if(optimized) result = stochasticEnumeration2(args);
			else result = stochasticEnumeration1(args);
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
	return residualConnectivity::main(argc, argv);
}

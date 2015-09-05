#include <boost/program_options.hpp>
#include "arguments.h"
#include "argumentsMPFR.h"
#include "monteCarloMethods/PMC.h"
#include "calculateFactorials.h"
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
				"This program estimates the probability that a random subgraph of a specified base graph is connected, using Permutation Monte Carlo. The random subgraph uses a vertex percolation model.\n\n"
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
		
		pmcArguments arguments(graph, randomSource);
		arguments.n = n;
		
		PMC(arguments);

		//Now calculate the estimate
		//First calculate factorials
		std::vector<mpz_class> factorials;
		calculateFactorials(factorials, (int)nVertices + 1);

		//Calculate powers of p and q (q = 1-p)
		std::vector<mpfr_class> powersP, powersQ;
		powersP.reserve(nVertices + 1);
		powersQ.reserve(nVertices + 1);
		powersP.push_back(1);
		powersQ.push_back(1);
		for (int i = 1; i < (int)nVertices + 1; i++)
		{
			powersP.push_back(powersP[i - 1] * opProbability);
			powersQ.push_back(powersQ[i - 1] * inopProbability);
		}

		//The case of no vertices has to be considered seperately. 
		mpfr_class estimate = powersQ[nVertices];
		for (int i = 0; i < (int)nVertices + 1; i++)
		{
			mpfr_class binomialCoefficient(mpz_class(factorials[nVertices] / (factorials[i] * factorials[nVertices - i])).str());
			estimate += ((binomialCoefficient * arguments.counts[i]) / n) * powersP[i] * powersQ[nVertices - i];
		}
		std::cout << "Estimate is " << estimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

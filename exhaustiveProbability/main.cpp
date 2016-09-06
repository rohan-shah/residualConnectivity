#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/program_options.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <iostream>
#include <iomanip>
#include "context.h"
#include "arguments.h"
#include "argumentsMPFR.h"
namespace residualConnectivity
{
	int main(int argc, char** argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			PROBABILITY_OPTION
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
				"This computes exactly the probability that a random subgraph is connected. The random subgraph uses a vertex percolation model.\n"
				"It takes the standard output of ExhaustiveSearch as input. Only feasible for small graphs. \n\n"
				;
			std::cout << options << std::endl;
			return 0;
		}
	
		std::string message;
		mpfr_class opProbability;
		if(!readSingleProbabilityString(variableMap, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		mpfr_class inopProbability = 1 - opProbability;

		std::vector<std::string> lines;
		std::string line;
		while(std::getline(std::cin, line))
		{
			lines.push_back(line);
		}
		std::size_t nLines = lines.size();
		std::size_t nVertices = nLines - 2;
		if(lines[0] != "Number of connected subgraphs with that number of points")
		{
			std::cout << "First line was invalid" << std::endl;
			return 0;
		}
		std::vector<mpz_class> graphCounts;
		for(std::vector<std::string>::iterator i = lines.begin()+1; i!=lines.end(); i++)
		{
			std::stringstream ss;
			ss << *i;
			int count;
			ss >> count;
			std::string colon;
			ss >> colon;
			mpz_class nGraphs;
			std::string nGraphsString;
			ss >> nGraphsString;
			nGraphs = mpz_class(nGraphsString);
			graphCounts.push_back(nGraphs);
		}
		std::cout << "Reliability probability is ";

		mpfr_class total = 0;

		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			mpfr_class opProbabilityPower = boost::multiprecision::pow(opProbability, i);
			mpfr_class inopProbabilityPower = boost::multiprecision::pow(inopProbability, nVertices - i);

			total += graphCounts[i] * opProbabilityPower * inopProbabilityPower;
		}
		std::cout << total.str() << std::endl;
		return 0;
	}
}
int main(int argc, char** argv)
{
	return residualConnectivity::main(argc, argv);
}

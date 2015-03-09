#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/program_options.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <iostream>
#include <iomanip>
#include "Context.h"
#include <mpir.h>
#include <mpirxx.h>
#include "arguments.h"
#include "argumentsMPIR.h"
namespace discreteGermGrain
{
	inline std::pair<int, int> vertexToPair(int vertex, int gridDimension)
	{
		return std::make_pair(vertex / gridDimension, vertex % gridDimension);
	}
	inline int pairToVertex(int first, int second, int gridDimension)
	{
		return (first * gridDimension) + second;
	}
	int main(int argc, char** argv)
	{
		mpf_set_default_prec(1024);

		boost::program_options::options_description options("Usage");
		options.add_options()
			("gridGraph", boost::program_options::value<int>(), "(int) The number of points for each dimension of the square grid")
			("graphFile", boost::program_options::value<std::string>(), "(string) The path to a graphml file. Incompatible with gridGraph")
			("probability", boost::program_options::value<std::string>(), "(float) The probability of opening a vertex")
			("help", "Display this message");

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
	
		mpf_class probability, probabilityComplement, one_mpf;
		mpf_set_d(one_mpf.get_mpf_t(), 1);
		if(!readProbabilityString(variableMap, probability))
		{
			return 0;
		}
		mpf_sub(probabilityComplement.get_mpf_t(), one_mpf.get_mpf_t(), probability.get_mpf_t());

		std::string message;
		Context context = Context::gridContext(1, mpf_get_d(probability.get_mpf_t()));
		if(!readContext(variableMap, context, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		std::size_t nVertices = boost::num_vertices(context.getGraph());

		std::vector<std::string> lines;
		std::string line;
		while(std::getline(std::cin, line))
		{
			lines.push_back(line);
		}
		std::size_t nLines = lines.size();
		{
			if(nLines != nVertices + 2)
			{
				std::cout << "Invalid number of lines of input entered" << std::endl;
				return 0;
			}
		}
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
			mpz_set_str(nGraphs.get_mpz_t(), nGraphsString.c_str(), 10);
			graphCounts.push_back(nGraphs);
		}
		std::cout << "Probability is ";

		mpf_class total = 0;

		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			mpf_class probabilityPower, probabilityComplementPower;
			mpf_pow_ui(probabilityPower.get_mpf_t(), probability.get_mpf_t(), i);
			mpf_pow_ui(probabilityComplementPower.get_mpf_t(), probabilityComplement.get_mpf_t(), nVertices - i);

			total += graphCounts[i] * probabilityPower * probabilityComplementPower;
			//parts[i] = graphCounts[i] * pow(probability, i) * pow(1 - probability, nVertices-i);
		}
		mp_exp_t exponent;
		char* resultCStr = mpf_get_str(NULL, &exponent, 10, 10, total.get_mpf_t());
		std::string resultStr = resultCStr;

		std::cout << resultStr.substr(0, 1) << "." << resultStr.substr(1, resultStr.size() - 2) << "e" << (exponent-1) << std::endl;
		free(resultCStr);
		return 0;
	}
}
int main(int argc, char** argv)
{
	return discreteGermGrain::main(argc, argv);
}

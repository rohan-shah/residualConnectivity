#include "exactMethods/exhaustiveSearch.h"
#include <boost/program_options.hpp>
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
			INPUT_GRAPH_OPTION
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
				"This program exhaustively enumerates the subgraphs of the specified graph. It counts the number of connected subgraphs with k vertices for k <= nVertices. The results are human-readable and written to standard output. Computationally demanding, and currently limited to nVertices <= 36. See ExhaustiveProbability.exe. \n\n";
			std::cout << options << std::endl;
			return 0;
		}
		std::string message;
		context contextObj = context::gridContext(1, 0.5);
		if(!readContext(variableMap, contextObj, 0.5, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		std::vector<counterType> sizeCounters;
		std::string error;
		bool result = exhaustiveSearch(contextObj.getGraph(), sizeCounters, error);
		if(!result)
		{
			std::cout << error << std::endl;
			return 0;
		}

		std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
		std::cout << "Number of connected subgraphs with that number of points" << std::endl;
		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			std::cout << std::setw(3) << i << ":  " << sizeCounters[i] << std::endl;
		}
		return 0;
	}
}
int main(int argc, char** argv)
{
	return residualConnectivity::main(argc, argv);
}


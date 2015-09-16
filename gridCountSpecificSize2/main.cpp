#include <iostream>
#include <boost/program_options.hpp>
#include "Context.h"
#include "arguments.h"
#include <boost/scoped_array.hpp>
#include <fstream>
#include "isSingleComponentWithRadius.h"
#include <cstdio>
#include <omp.h>
#include <boost/graph/connected_components.hpp>
#include "depth_first_search_restricted.hpp"
#include "connected_components_restricted.hpp"
#ifdef _MSC_VER
	#include <intrin.h>
#endif
namespace residualConnectivity
{
	mpz_class countSubgraphsBySize(int gridDimension, int size, const Context& context)
	{
		std::size_t connected = 0;
		std::vector<int> connectedComponents;
		connectedComponents.resize(gridDimension*gridDimension);
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;

		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colors;
		colors.resize(gridDimension*gridDimension);
		std::vector<Context::inputGraph::vertex_descriptor> initialPoints;
		initialPoints.reserve(gridDimension*gridDimension);

		//first permunation
		std::size_t currentPermutation = (1ULL << size) - 1;
		std::size_t lastPermutation = ((1ULL << (gridDimension*gridDimension)) - 1) - ((1ULL << (gridDimension*gridDimension-size)) - 1);
		do
		{
			initialPoints.clear();
			for(int i = 0; i < gridDimension*gridDimension; i++)
			{
				if(currentPermutation & (1ULL << i))
				{
					colors[i] = Color::white();
					initialPoints.push_back(i);
				}
				else colors[i] = Color::black();
			}
			int nComponents = boost::connected_components_restricted(context.getGraph(), &(connectedComponents[0]), &(colors[0]), stack, initialPoints);
			if(nComponents <= 1)
			{
				connected++;
			}
			if(currentPermutation == lastPermutation) break;
			std::size_t t = currentPermutation | (currentPermutation-1);
#ifdef _MSC_VER
			unsigned long index;
			_BitScanForward64(&index, currentPermutation);
			currentPermutation = (t + 1) | (((~t & -~t) - 1) >> (index + 1));
#else
			currentPermutation =(t + 1) | (((~t & -~t) - 1) >> (__builtin_ctzll(currentPermutation) + 1));
#endif
		}
		while(true);
		return connected;

	}
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			GRID_GRAPH_OPTION
			("size", boost::program_options::value<int>(), "(int) Count the number of connected subgraphs with this number of vertices. ")
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
				"This uses a transfer matrix approach to compute the connectivity probability. It can also count the total number of connected subgraphs."
				"\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		int gridDimension;
		std::string message;
		if(!readGridGraph(variableMap, gridDimension, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		if(gridDimension > 8)
		{
			std::cout << "Largest allowable value of input `gridDimension' is 8" << std::endl;
			return 0;
		}
		if(variableMap.count("size") < 1)
		{
			std::cout << "Input `size' is required" << std::endl;
			return 0;
		}
		int size = variableMap["size"].as<int>();
		if(size < 0 || size > gridDimension * gridDimension)
		{
			std::cout << "Input `size' had an invalid value" << std::endl;
			return 0;
		}
		Context context = Context::gridContext(gridDimension, 0.5);
		mpz_class count = countSubgraphsBySize(gridDimension, size, context);
		std::cout << "Number of connected subgraphs of the " << gridDimension << " x " << gridDimension << " grid graph with " << size << " vertices was " << count << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}

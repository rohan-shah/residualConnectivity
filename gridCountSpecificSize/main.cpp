#include <iostream>
#include <boost/program_options.hpp>
#include "Context.h"
#include "arguments.h"
#include <boost/scoped_array.hpp>
#include <fstream>
#include "isSingleComponentWithRadius.h"
#include <cstdio>
#include <omp.h>
namespace discreteGermGrain
{
	inline int countSetBits(std::size_t i)
	{
		i = i - ((i >> 1) & 0x5555555555555555ULL);
		i = (i & 0x3333333333333333ULL) + ((i >> 2) & 0x3333333333333333ULL);
		return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FULL) * 0x101010101010101ULL) >> 56);
	}
	mpz_class countMultiThreaded(int gridDimension, int size, const Context& context, std::string cacheFile)
	{
		std::size_t connected = 0;
		std::size_t maxValue = (1ULL << (gridDimension*gridDimension));
		std::size_t increment = (1ULL << 32);
		std::size_t start = 0;
		{
			std::ifstream cacheStream(cacheFile.c_str());
			if(cacheStream)
			{
				cacheStream >> start;
				cacheStream >> connected;
			}
		}
		#pragma omp parallel firstprivate(start)
		{
			std::vector<int> connectedComponents;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			std::vector<vertexState> state;
			state.resize(gridDimension*gridDimension);
			for(; start < maxValue; start += increment)
			{
				std::size_t end = std::min(start + increment, maxValue);
				#pragma omp for schedule(static, 1) 
				for(std::size_t counter = start; counter < end; counter++)
				{
					if(countSetBits(counter) == size)
					{
						for(int i = 0; i < gridDimension*gridDimension; i++)
						{
							if(counter & (1ULL << i)) state[i].state = FIXED_ON;
							else state[i].state = FIXED_OFF;
						}
						if(isSingleComponentPossible(context, &(state[0]), connectedComponents, stack))
						{
							#pragma omp critical
							connected++;
						}
					}
				}
				if(omp_get_thread_num() == 0)
				{
					std::ofstream cacheStream(cacheFile.c_str());
					cacheStream << end << std::endl << connected;
				}
			}
		}
		remove(cacheFile.c_str());
		return connected;
	}
	mpz_class countSingleThreaded(int gridDimension, int size, const Context& context)
	{
		std::size_t connected = 0;
		std::size_t maxValue = (1ULL << gridDimension*gridDimension);
		{
			std::vector<int> connectedComponents;
			boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
			std::vector<vertexState> state;
			state.resize(gridDimension*gridDimension);
			for(std::size_t counter = 0; counter < maxValue; counter++)
			{
				if(countSetBits(counter) == size)
				{
					for(int i = 0; i < gridDimension*gridDimension; i++)
					{
						if(counter & (1 << i)) state[i].state = FIXED_ON;
						else state[i].state = FIXED_OFF;
					}
					if(isSingleComponentPossible(context, &(state[0]), connectedComponents, stack))
					{
						connected++;
					}
				}
			}
		}
		return connected;

	}
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			GRID_GRAPH_OPTION
			("size", boost::program_options::value<int>(), "(int) Count the number of connected subgraphs with this number of vertices. ")
			("multithreaded", boost::program_options::value<bool>()->default_value(true)->implicit_value(true), "(flag) Should we use multithreading?")
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
		bool multithreaded = variableMap["multithreaded"].as<bool>();
		Context context = Context::gridContext(gridDimension, 0.5);
		mpz_class count;
		if(multithreaded)
		{
			count = countMultiThreaded(gridDimension, size, context, "cacheFile");
		}
		else
		{
			count = countSingleThreaded(gridDimension, size, context);
		}
		std::cout << "Number of connected subgraphs of the " << gridDimension << " x " << gridDimension << " grid graph with " << size << " vertices was " << count << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

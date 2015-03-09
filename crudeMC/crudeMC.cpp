#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "DiscreteGermGrainObs.h"
#include <boost/algorithm/string.hpp>
#include "DiscreteGermGrainSubObs.h"
#include "isSingleComponentWithRadius.h"
#include "Context.h"
#include <boost/graph/graphml.hpp>
#include "arguments.h"
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			("gridGraph", boost::program_options::value<int>(), "(int) The dimension of the square grid graph to use. Incompatible with graphFile and torusGraph. ")
			("graphFile", boost::program_options::value<std::string>(), "(string) The path to a graphml file. Incompatible with gridGraph and torusGraph. ")
			("torusGraph", boost::program_options::value<int>(), "(int) The dimension of the torus graph to use. Incompatible with gridGraph and graphFile. ")
			("probability", boost::program_options::value<float>(), "(float) The probability that a vertex is open")
			("n", boost::program_options::value<int>(), "(int) The number of simulations to perform")
			("seed", boost::program_options::value<int>(), "(int) The random seed used to generate the random graphs")
			("splitting", boost::program_options::value<int>(), "(int) Estimate splitting level probabilities")
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
		float probability;
		if(!readProbability(variableMap, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		Context context = Context::gridContext(1, probability);
		if(!readContext(variableMap, context, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		int splitting = 0;
		if(variableMap.count("splitting") > 0)
		{
			splitting = variableMap["splitting"].as<int>();
		}

		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		std::vector<DiscreteGermGrainObs> observations;
		std::vector<int> scratchMemory;
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		boost::scoped_array<int> counters(new int[splitting + 1]);
		memset(counters.get(), 0, sizeof(int)*(splitting+1));

		//The cumulative states, in case we decide to use the --splitting option
		std::vector<vertexState> cumulativeStates(context.nVertices());
		for(int i = 0; i < n; i++)
		{
			DiscreteGermGrainObs obs(context, randomSource);
			if(splitting > 0)
			{
				std::fill(cumulativeStates.begin(), cumulativeStates.end(), vertexState::unfixed_off());
				for(int radius = splitting; radius >= 0; radius--)
				{
					const vertexState* newState = obs.getSubObservation(radius).getState();
					for(std::size_t j = 0; j < context.nVertices(); j++)
					{
						if(newState[j].state & FIXED_MASK)
						{
							cumulativeStates[j] = newState[j];
						}
					}
					if(isSingleComponentPossible(context, &(cumulativeStates[0]), scratchMemory, stack))
					{
						counters[radius]++;
					}
					//To follow the splitting logic we need to stop at the first radius where it fails
					else break;
				}
			}
			if(isSingleComponentAllOn(context, obs.getState(), scratchMemory, stack))
			{
				observations.push_back(std::move(obs));
			}
		}
		std::cout << observations.size() << " / " << n << " = " << ((float)observations.size() / (float)n) << " had one connected component" << std::endl;

		if(splitting > 0)
		{
			std::size_t previousCount = n;
			for(int radius = splitting; radius >= 0; radius--)
			{
				std::cout << "Step " << (splitting - radius) << ", probability " << counters[radius] << " / " << previousCount << " = " << ((double)counters[radius] / (double)previousCount) << std::endl;
				previousCount = counters[radius];
			}
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

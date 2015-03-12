#include "Context.h"
#include "arguments.h"
#include "argumentsMPFR.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/program_options.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/graph/connected_components.hpp>
#include "connected_components_restricted.hpp"
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			INPUT_GRAPH_OPTION
			PROBABILITY_OPTION
			SEED_OPTION
			N_OPTION
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
				"This uses a splitting approach to estimate the probability that a random subgraph is connected. The random subgraph uses a vertex percolation model."
				"\n\n"
				;
			std::cout << options << std::endl;
			return 0;
		}

		std::string message;
		mpfr_class opProbability, inopProbability;
		if(!readProbabilityString(variableMap, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		inopProbability = 1 - opProbability;
		Context context = Context::gridContext(1, opProbability);
		if(!readContext(variableMap, context, opProbability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		std::size_t nVertices = context.nVertices();
		if(nVertices <= 1)
		{
			std::cout << "Estimate is 1" << std::endl;
			return 0;
		}
		boost::mt19937 randomSource;
		if(variableMap.count("seed") > 0)
		{
			randomSource.seed(variableMap["seed"].as<int>());
		}
		//Cache powers of opProbability and inopProbability
		std::vector<mpfr_class> inopProbabilityPowers(nVertices+1), opProbabilityPowers(nVertices+1);
		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			inopProbabilityPowers[i] = boost::multiprecision::pow(inopProbability, i);
			opProbabilityPowers[i] = boost::multiprecision::pow(opProbability, i);
		}
		//Truncated geometric distribution for use later on
		std::vector<mpfr_class> truncatedGeometric(nVertices+1);
		truncatedGeometric[0] = inopProbability;
		for(std::size_t i = 1; i < nVertices+1; i++)
		{
			truncatedGeometric[i] = truncatedGeometric[i-1] + inopProbability * opProbabilityPowers[i];
		}
		//unifrom distribution
		boost::random::uniform_01<double> uniformDistribution;
		const Context::inputGraph& graph = context.getGraph();

		//scratch memory, to determine whether the base graph is connected
		std::vector<int> components(nVertices);
		//more scratch memory
		std::vector<typename boost::graph_traits<Context::inputGraph>::vertex_descriptor> allVertices;
		for(std::size_t i = 0; i < nVertices; i++)allVertices.push_back(i);
		bool totalGraphConnected = boost::connected_components(context.getGraph(), &(components[0])) == 1;

		std::vector<boost::default_color_type> colorVector(nVertices);
		typedef boost::color_traits<boost::default_color_type> Color;

		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;

		std::vector<bool> iterativeVertexStates(nVertices);
		int n;
		if(!readN(variableMap, n))
		{
			std::cout << "Error reading input `n'" << std::endl;
			return 0;
		}
		mpfr_class sum = 0;
		for(int i = 0; i < n; i++)
		{
			iterativeVertexStates.clear();
			iterativeVertexStates.resize(nVertices, true);

			mpfr_class currentPart = 0;
			mpfr_class currentMultiple = 1;
			std::size_t currentPosition = 0;
			bool currentConnected = totalGraphConnected;
			while(true)
			{
				//Terminating conditions. First, if we have currentPart == nVertices - 1 then we're done. 
				if(currentPosition == nVertices)
				{
					if(currentConnected) currentPart += currentMultiple;
					break;
				}
				//recursive part
				if(currentConnected)
				{
					currentPart += currentMultiple * opProbabilityPowers[nVertices - currentPosition];
				}
				mpfr_class extraPart = (1 - opProbabilityPowers[nVertices - currentPosition]);
				currentMultiple *= extraPart;

				mpfr_class randomValue = extraPart * uniformDistribution(randomSource);
				std::size_t increment = 0;
				for(; increment < nVertices - currentPosition; increment++)
				{
					if(randomValue <= truncatedGeometric[increment]) break;
				}
				//mark currentPosition + increment as off. 
				iterativeVertexStates[currentPosition + increment] = false;
				currentPosition += increment;
				currentPosition++;
				//Set the colour vector appropriately
				for(std::size_t j = 0; j < nVertices; j++)
				{
					if(iterativeVertexStates[j]) colorVector[j] = Color::white();
					else colorVector[j] = Color::black();
				}
				//update the currentConnected variable
				currentConnected = boost::connected_components_restricted(graph, &(components[0]), &(colorVector[0]), stack, allVertices) <= 1;
			}
			sum += currentPart;
		}
		std::cout << "Reliability estimate was " << mpfr_class(sum/n).str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

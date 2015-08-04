#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
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
		std::vector<unsigned long long> counts(nVertices, 0ULL);

		typedef boost::graph_traits<Context::inputGraph>::vertex_descriptor Vertex;
		typedef boost::graph_traits<Context::inputGraph>::vertices_size_type VertexIndex;
		std::vector<VertexIndex> rank(boost::num_vertices(graph));
		std::vector<Vertex> parent(boost::num_vertices(graph));
		typedef VertexIndex* Rank;
		typedef Vertex* Parent;
		boost::disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);



		std::vector<int> permutation(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
		boost::random_number_generator<boost::mt19937> generator(randomSource);
		for (int i = 0; i < n; i++)
		{
			boost::range::random_shuffle(permutation, generator);
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

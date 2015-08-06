#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "arguments.h"
#include "argumentsMPFR.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
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
		std::vector<unsigned long long> counts(nVertices+1, 0ULL);
		counts[0] = (unsigned long long)n;

		typedef boost::graph_traits<Context::inputGraph>::vertex_descriptor Vertex;
		typedef boost::graph_traits<Context::inputGraph>::vertices_size_type VertexIndex;
		std::vector<VertexIndex> rank(boost::num_vertices(graph), 0);
		std::vector<Vertex> parent(boost::num_vertices(graph), 0);
		typedef VertexIndex* Rank;
		typedef Vertex* Parent;

		std::vector<int> permutation(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
		boost::random_number_generator<boost::mt19937> generator(randomSource);

		std::vector<int> scratchMemory;
		scratchMemory.reserve(nVertices);
		//Keep track of which vertices have already been added
		std::vector<bool> alreadyPresent(nVertices, false);
		for (int i = 0; i < n; i++)
		{
			boost::range::random_shuffle(permutation, generator);
			boost::disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);
			std::fill(alreadyPresent.begin(), alreadyPresent.end(), false);
			for (int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
			{
				ds.make_set(permutation[vertexCounter]);
				alreadyPresent[permutation[vertexCounter]] = true;
				Context::inputGraph::out_edge_iterator current, last;
				boost::tie(current, last) = boost::out_edges(permutation[vertexCounter], graph);
				for (; current != last; current++)
				{
					if (alreadyPresent[current->m_target])
					{
						scratchMemory.push_back((int)current->m_target);
					}
				}
				for (std::vector<int>::iterator current = scratchMemory.begin(); current != scratchMemory.end(); current++)
				{
					ds.union_set((Vertex)permutation[vertexCounter], (Vertex)*current);
				}
				scratchMemory.clear();
				int components = 0;
				for (int j = 0; j < vertexCounter + 1; j++)
				{
					components += (parent[permutation[j]] == permutation[j]);
					if (components > 1) break;
				}
				//if (ds.count_sets(&permutation[0], &permutation[0] + vertexCounter + 1) == 1)
				if (components == 1)
				{
					counts[vertexCounter+1]++;
				}
			}
		}

		//Now calculate the estimate
		//First calculate factorials
		std::vector<mpz_class> factorials;
		calculateFactorials(factorials, nVertices + 1);

		//Calculate powers of p and q (q = 1-p)
		std::vector<mpfr_class> powersP, powersQ;
		powersP.reserve(nVertices + 1);
		powersQ.reserve(nVertices + 1);
		powersP.push_back(1);
		powersQ.push_back(1);
		for (int i = 1; i < nVertices + 1; i++)
		{
			powersP.push_back(powersP[i - 1] * opProbability);
			powersQ.push_back(powersQ[i - 1] * inopProbability);
		}

		//The case of no vertices has to be considered seperately. 
		mpfr_class estimate = powersQ[nVertices];
		for (int i = 0; i < (int)nVertices + 1; i++)
		{
			mpfr_class binomialCoefficient(mpz_class(factorials[nVertices] / (factorials[i] * factorials[nVertices - i])).str());
			estimate += ((binomialCoefficient * counts[i]) / n) * powersP[i] * powersQ[nVertices - i];
		}
		std::cout << "Estimate is " << estimate.str() << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

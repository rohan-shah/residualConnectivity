#include <boost/random/mersenne_twister.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
#include "monteCarloMethods/PMC.h"
namespace residualConnectivity
{
	pmcArguments::pmcArguments(const context::inputGraph& graph, boost::mt19937& randomSource)
		:graph(graph), randomSource(randomSource)
	{}
	bool PMC(pmcArguments& args)
	{
		const context::inputGraph& graph = args.graph;
		std::size_t nVertices = boost::num_vertices(graph);
		std::vector<unsigned long long>& counts = args.counts;
		counts.clear();
		counts.resize(nVertices+1, 0ULL);
		counts[0] = (unsigned long long)args.n;

		typedef boost::graph_traits<context::inputGraph>::vertex_descriptor Vertex;
		typedef boost::graph_traits<context::inputGraph>::vertices_size_type VertexIndex;
		std::vector<VertexIndex> rank(boost::num_vertices(graph), 0);
		std::vector<Vertex> parent(boost::num_vertices(graph), 0);
		typedef VertexIndex* Rank;
		typedef Vertex* Parent;

		std::vector<int> permutation(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
		boost::random_number_generator<boost::mt19937> generator(args.randomSource);

		std::vector<int> scratchMemory;
		scratchMemory.reserve(nVertices);
		//Keep track of which vertices have already been added
		std::vector<bool> alreadyPresent(nVertices, false);
		for (int i = 0; i < args.n; i++)
		{
			boost::range::random_shuffle(permutation, generator);
			boost::disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);
			std::fill(alreadyPresent.begin(), alreadyPresent.end(), false);
			for (int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
			{
				ds.make_set(permutation[vertexCounter]);
				alreadyPresent[permutation[vertexCounter]] = true;
				context::inputGraph::out_edge_iterator current, last;
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
					components += ((int)parent[permutation[j]] == permutation[j]);
					if (components > 1) break;
				}
				//if (ds.count_sets(&permutation[0], &permutation[0] + vertexCounter + 1) == 1)
				if (components == 1)
				{
					counts[vertexCounter+1]++;
				}
			}
		}
		return true;
	}
}

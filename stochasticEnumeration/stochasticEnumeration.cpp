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
			HELP_OPTION
			("vertexCount", boost::program_options::value<int>(), "(int) Number of UP vertices")
			;

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
				"This program estimates the probability that a random subgraph of a specified base graph is connected, using Stochastic Enumeration. The random subgraph uses a vertex percolation model.\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		int n;
		if(!readN(variableMap, n))
		{
			return 0;
		}
		if(n <= 2)
		{
			std::cout << "Input n must be at least 2" << std::endl;
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
		boost::random_number_generator<boost::mt19937> generator(randomSource);

		if (variableMap.count("vertexCount") < 1)
		{
			std::cout << "Input vertexCount is required" << std::endl;
			return 0;
		}
		int vertexCount = variableMap["vertexCount"].as<int>();
		if (vertexCount <= 1 || vertexCount > nVertices)
		{
			std::cout << "Input vertexCount must be a value in [2, nVertices]" << std::endl;
			return 0;
		}

		std::vector<int> nActiveVertices(n, 0);
		std::vector<int> rank(n*nVertices, 0);
		std::vector<int> parent(n*nVertices, 0);
		std::vector<bool> alreadyPresent(n*nVertices, false);

		std::vector<int> copiedNActiveVertices(n, 0);
		std::vector<int> copiedRank(n*nVertices, 0);
		std::vector<int> copiedParent(n*nVertices, 0);
		std::vector<bool> copiedAlreadyPresent(n*nVertices, false);
		//Initialize the algorithm by having the first vertex both up and down. 
		int nParticles = 2;
		{
			boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
			ds.make_set(0);
		}
		nActiveVertices[0] = 1;
		nActiveVertices[1] = 0;
		alreadyPresent[0] = true;

		mpfr_class total = 0;
		mpfr_class multiple = 1;
		//Scratch memory
		std::vector<std::pair<int, int> > scratch;
		scratch.reserve(nParticles*2);

		for(int vertexCounter = 1; vertexCounter < (int)nVertices; vertexCounter++)
		{
			scratch.clear();
			if(nParticles == 0) break;
			int nConnected = 0;
			for(int particleCounter = 0; particleCounter < nParticles; particleCounter++)
			{
				//No more vertices will be added to this particle
				if(nActiveVertices[particleCounter] == vertexCount)
				{
					boost::disjoint_sets<int*, int*> ds(&rank[particleCounter*nVertices], &parent[particleCounter*nVertices]);
					int connectedComponents = 0;
					for(int i = 0; i < nVertices; i++)
					{
						if(alreadyPresent[particleCounter*nVertices+i]) connectedComponents += parent[particleCounter*nVertices+i] == i;
					}
					nConnected += connectedComponents == 1;
				}
				//All remaining vertices will be added to this particle
				else if(nActiveVertices[particleCounter] + nVertices - vertexCounter == vertexCount)
				{
					boost::disjoint_sets<int*, int*> ds(&rank[particleCounter*nVertices], &parent[particleCounter*nVertices]);
					//Add in remaining vertices
					for(int k = vertexCounter; k < (int)nVertices; k++)
					{
						ds.make_set(k);
						Context::inputGraph::out_edge_iterator current, last;
						boost::tie(current, last) = boost::out_edges(k, graph);
						for(; current != last; current++)
						{
							int other = (int)current->m_target;
							if(alreadyPresent[particleCounter*nVertices+other] || (other <= k && other >= vertexCounter))
							{
								ds.union_set(other, k);
							}
						}
					}
					int connectedComponents = 0;
					for(int i = 0; i < nVertices; i++)
					{
						if(alreadyPresent[particleCounter*nVertices+i] || i >= vertexCounter) connectedComponents += parent[particleCounter*nVertices+i] == i;
					}
					nConnected += (connectedComponents == 1);
				}
				else
				{
					scratch.push_back(std::make_pair(particleCounter, 0));
					scratch.push_back(std::make_pair(particleCounter, 1));
				}
			}
			int newParticlesCount = std::min(n, (int)scratch.size());
			boost::random_shuffle(scratch, generator);
			total += multiple * nConnected;
			multiple *= mpfr_class(scratch.size()) / mpfr_class(newParticlesCount);

			for(int particleCounter = 0; particleCounter < newParticlesCount; particleCounter++)
			{
				std::pair<int, int> newParticleData = *scratch.rbegin();
				scratch.pop_back();

				//Same number of active vertices, or maybe one more. 
				copiedNActiveVertices[particleCounter] = nActiveVertices[newParticleData.first] + newParticleData.second;
				//Copy rank data
				std::copy(rank.begin() + (newParticleData.first*nVertices), rank.begin() + (newParticleData.first+1)*nVertices, copiedRank.begin() + particleCounter*nVertices);
				//copy parent data
				std::copy(parent.begin() + (newParticleData.first*nVertices), parent.begin() + (newParticleData.first+1)*nVertices, copiedParent.begin() + (particleCounter*nVertices));
				//copy already present data
				std::copy(alreadyPresent.begin() + newParticleData.first*nVertices, alreadyPresent.begin() + (newParticleData.first+1)*nVertices, copiedAlreadyPresent.begin() + particleCounter*nVertices);
				//Add extra vertex, if required
				if(newParticleData.second)
				{
					copiedAlreadyPresent[particleCounter * nVertices + vertexCounter] = true;

					boost::disjoint_sets<int*, int*> ds(&copiedRank[particleCounter*nVertices], &copiedParent[particleCounter*nVertices]);
					ds.make_set(vertexCounter);

					Context::inputGraph::out_edge_iterator current, last;
					boost::tie(current, last) = boost::out_edges(vertexCounter, graph);
					for(; current != last; current++)
					{
						if(copiedAlreadyPresent[particleCounter*nVertices + current->m_target])
						{
							ds.union_set(vertexCounter, (int)current->m_target);
						}
					}
				}
			}

			nActiveVertices.swap(copiedNActiveVertices);
			rank.swap(copiedRank);
			parent.swap(copiedParent);
			alreadyPresent.swap(copiedAlreadyPresent);
			nParticles = newParticlesCount;
		}

		std::cout << "Estimate is " << total << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}

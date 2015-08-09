#include "stochasticEnumerationLib.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
#include <boost/graph/connected_components.hpp>
#include "connected_components_restricted.hpp"
namespace discreteGermGrain
{
	bool stochasticEnumeration2(stochasticEnumerationArgs& args)
	{
		const Context::inputGraph& graph = args.graph;
		std::size_t nVertices = boost::num_vertices(graph);
		boost::random_number_generator<boost::mt19937> generator(args.randomSource);

		int vertexCount = args.vertexCount;
		if (vertexCount > nVertices)
		{
			args.message = "Input vertexCount cannot be larger than the number of vertices";
			return false;
		}
		if(vertexCount == 0)
		{
			args.estimate = 1; return true;
		}
		else if(vertexCount == 1)
		{
			args.estimate = nVertices; 
			return true;
		}

		int n = args.n;
		if(n < 2)
		{
			args.message = "Input n must be greater than 2";
			return false;
		}

		std::vector<int> nActiveVertices(n, 0);
		std::vector<int> anActiveVertex(n, 0);
		std::vector<int> rank(n*nVertices, 0);
		std::vector<int> parent(n*nVertices, 0);
		std::vector<bool> alreadyPresent(n*nVertices, false);

		std::vector<int> copiedNActiveVertices(n, 0);
		std::vector<int> copiedAnActiveVertex(n, 0);
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
		std::vector<std::pair<int, int> > scratchPairs;
		scratchPairs.reserve(nParticles*2);

		//Scratch memory for depth first search
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colors(nVertices, Color::black());
		std::vector<int> connectedComponentVector(nVertices);
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		std::vector<Context::inputGraph::vertex_descriptor> initialVertex(1, 0);

		for(int vertexCounter = 1; vertexCounter < (int)nVertices; vertexCounter++)
		{
			scratchPairs.clear();
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
				//In this case we have a choice of whether or not to take this vertex as active. So we check that
				//1. This vertex is contained in the connected component attached to anActiveVertex[vertexCounter]
				//2. The connected component attached to anActiveVertex has at least vertexCount vertices
				else
				{
					bool optionalVertex = true;
					if(nActiveVertices[particleCounter] > 0) 
					{
						//Colours are initially all white
						std::fill(colors.begin(), colors.end(), Color::white());
						//Those that are fixed at OFF are marked as black
						for(int i = 0; i < vertexCounter; i++) 
						{
							if(!alreadyPresent[particleCounter*nVertices+i]) colors[i] = Color::black();
						}
						initialVertex[0] = anActiveVertex[particleCounter];
						boost::connected_components_restricted(graph, &(connectedComponentVector[0]), &(colors[0]), stack, initialVertex);
						int importantComponent = connectedComponentVector[initialVertex[0]];
						int maximumSize = 0;
						for(int i = 0; i < nVertices; i++)
						{
							if(connectedComponentVector[i] == importantComponent) maximumSize++;
						}
						//If (no matter how many more vertices we add) we cannot reach the desired component size, then this corresponds to a leaf with zero children, and 0 cost. So skip this one completely
						if(maximumSize < vertexCount) continue;
						//If the vertex under consideration is in the same component as initialVertex[0], then it's optional
						if(connectedComponentVector[vertexCounter] == importantComponent) optionalVertex = true;
						//Otherwise it must be off, so it's not optional. 
						else optionalVertex = false;
					}
					if(optionalVertex)
					{
						scratchPairs.push_back(std::make_pair(particleCounter, 0));
						scratchPairs.push_back(std::make_pair(particleCounter, 1));
					}
					else
					{
						scratchPairs.push_back(std::make_pair(particleCounter, 0));
					}
				}
			}
			int newParticlesCount = std::min(n, (int)scratchPairs.size());
			boost::random_shuffle(scratchPairs, generator);
			total += multiple * nConnected;
			multiple *= mpfr_class(scratchPairs.size()) / mpfr_class(newParticlesCount);

			for(int particleCounter = 0; particleCounter < newParticlesCount; particleCounter++)
			{
				std::pair<int, int> newParticleData = *scratchPairs.rbegin();
				scratchPairs.pop_back();

				//Same number of active vertices, or maybe one more. 
				copiedNActiveVertices[particleCounter] = nActiveVertices[newParticleData.first] + newParticleData.second;
				//Copy rank data
				std::copy(rank.begin() + (newParticleData.first*nVertices), rank.begin() + (newParticleData.first+1)*nVertices, copiedRank.begin() + particleCounter*nVertices);
				//copy parent data
				std::copy(parent.begin() + (newParticleData.first*nVertices), parent.begin() + (newParticleData.first+1)*nVertices, copiedParent.begin() + (particleCounter*nVertices));
				//copy already present data
				std::copy(alreadyPresent.begin() + newParticleData.first*nVertices, alreadyPresent.begin() + (newParticleData.first+1)*nVertices, copiedAlreadyPresent.begin() + particleCounter*nVertices);
				//Copy data about a (random) active vertex (for use in depth-first search)
				copiedAnActiveVertex[particleCounter] = anActiveVertex[newParticleData.first];
				//Add extra vertex, if required
				if(newParticleData.second)
				{
					copiedAlreadyPresent[particleCounter * nVertices + vertexCounter] = true;
					copiedAnActiveVertex[particleCounter] = vertexCounter;

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
			anActiveVertex.swap(copiedAnActiveVertex);
			nParticles = newParticlesCount;
		}

		args.estimate = total;
		return true;
	}
}
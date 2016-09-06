#include "monteCarloMethods/stochasticEnumeration.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
#include <boost/graph/connected_components.hpp>
#include "connected_components_restricted.hpp"
namespace residualConnectivity
{
	bool stochasticEnumeration2(stochasticEnumerationArgs& args)
	{
		const context::inputGraph& graph = args.graph;
		const std::size_t nVertices = boost::num_vertices(graph);
		boost::random_number_generator<boost::mt19937> generator(args.randomSource);

		int vertexCount = args.vertexCount;
		if (vertexCount > (int)nVertices)
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

		int nPermutations = args.nPermutations;
		if (nPermutations < 1)
		{
			args.message = "Input nPermutations must be at least 1";
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

		//Scratch memory
		std::vector<std::pair<int, int> > scratchPairs;
		scratchPairs.reserve(n * 2);

		//Scratch memory for depth first search
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colors(nVertices, Color::black());
		std::vector<int> connectedComponentVector(nVertices);
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
		std::vector<context::inputGraph::vertex_descriptor> initialVertex(0);
		initialVertex.reserve(nVertices);

		//The permutation is initially the identity. This means that if we set nPermutations = 1 we will get the same permutation for every seed. 
		std::vector<int> permutation(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
		mpfr_class totalOverPermutations = 0;
		for(int permutationCounter = 0; permutationCounter < args.nPermutations; permutationCounter++)
		{
			//It seems the easiest thing is to actually reorder the vertices within the graph (because we rely on being able to easily determine which vertices are AFTER the current vertex)
			context::inputGraph reorderedGraph(nVertices);
			{
				context::inputGraph::edge_iterator current, end;
				boost::tie(current, end) = boost::edges(graph);
				for(; current != end; current++)
				{
					boost::add_edge(permutation[current->m_source], permutation[current->m_target], reorderedGraph);
				}
			}

			//Initialize the algorithm by having the first vertex both up and down. 
			int nParticles = 2;
			{
				boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
				ds.make_set(0);
			}
			nActiveVertices[0] = 1;
			nActiveVertices[1] = 0;
			alreadyPresent[0] = true;
			alreadyPresent[nVertices] = false;
			anActiveVertex[0] = 0;
			std::fill(rank.begin(), rank.end(), 0);
			std::fill(parent.begin(), parent.end(), 0);

			mpfr_class total = 0;
			mpfr_class multiple = 1;

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
						for(int i = 0; i < (int)vertexCounter; i++)
						{
							if(alreadyPresent[particleCounter*nVertices+i]) connectedComponents += parent[particleCounter*nVertices+i] == i;
						}
						nConnected += connectedComponents == 1;
					}
					//All remaining vertices will be added to this particle
					else if(nActiveVertices[particleCounter] + (int)nVertices - vertexCounter == vertexCount)
					{
						boost::disjoint_sets<int*, int*> ds(&rank[particleCounter*nVertices], &parent[particleCounter*nVertices]);
						//Add in remaining vertices
						for(int k = vertexCounter; k < (int)nVertices; k++)
						{
							ds.make_set(k);
							context::inputGraph::out_edge_iterator current, last;
							boost::tie(current, last) = boost::out_edges(k, reorderedGraph);
							for(; current != last; current++)
							{
								int other = (int)current->m_target;
								if((alreadyPresent[particleCounter*nVertices+other] && other < vertexCounter) || (other <= k && other >= vertexCounter))
								{
									ds.union_set(other, k);
								}
							}
						}
						int connectedComponents = 0;
						for(int i = 0; i < (int)nVertices; i++)
						{
							if(alreadyPresent[particleCounter*nVertices+i] || i >= vertexCounter) connectedComponents += parent[particleCounter*nVertices+i] == i;
						}
						nConnected += (connectedComponents == 1);
					}
					//In this case we have a choice of whether or not to take this vertex as active. So we check that
					//1. This vertex is contained in the connected component attached to anActiveVertex[particleCounter]
					//2. The connected component attached to anActiveVertex has at least vertexCount vertices
					else
					{
						bool possiblyOn = true, possiblyOff = true;
						if(nActiveVertices[particleCounter] > 0) 
						{
							//Colours are initially all white
							std::fill(colors.begin(), colors.end(), Color::white());
							//Those that are fixed at OFF are marked as black
							for(int i = 0; i < vertexCounter; i++) 
							{
								if(!alreadyPresent[particleCounter*nVertices+i]) colors[i] = Color::black();
							}
							initialVertex.clear();
							initialVertex.push_back(anActiveVertex[particleCounter]);
							//First the maximum size with this vertex on
							std::fill(connectedComponentVector.begin(), connectedComponentVector.end(), -1);
							boost::connected_components_restricted(reorderedGraph, &(connectedComponentVector[0]), &(colors[0]), stack, initialVertex);
							int importantComponent = connectedComponentVector[initialVertex[0]];
							int maximumSizeOn = 0;
							for(int i = 0; i < (int)nVertices; i++)
							{
								if(connectedComponentVector[i] == importantComponent) maximumSizeOn++;
							}
							//If (no matter how many more vertices we add) we cannot reach the desired component size, then this corresponds to a leaf with zero children, and 0 cost. So skip this one completely
							if(maximumSizeOn < vertexCount) continue;
							//If the vertex under consideration is in a different connected component then it can't be on
							if (connectedComponentVector[vertexCounter] != importantComponent)
							{
								possiblyOn = false;
							}
							int maximumSizeOff = 0;
							std::fill(connectedComponentVector.begin(), connectedComponentVector.end(), -1);
							std::fill(colors.begin(), colors.end(), Color::white());
							initialVertex.clear();
							//Those that are fixed at OFF are marked as black
							for (int i = 0; i < vertexCounter; i++)
							{
								if (!alreadyPresent[particleCounter*nVertices + i]) colors[i] = Color::black();
								else initialVertex.push_back(i);
							}
							colors[vertexCounter] = Color::black();
							boost::connected_components_restricted(reorderedGraph, &(connectedComponentVector[0]), &(colors[0]), stack, initialVertex);
							bool multipleComponents = false;
							for (int i = 0; i < (int)nVertices; i++)
							{
								if (connectedComponentVector[i] == importantComponent) maximumSizeOff++;
								else if (i < vertexCounter && alreadyPresent[particleCounter*nVertices + i]) multipleComponents = true;
							}
							if (maximumSizeOff < vertexCount || multipleComponents) possiblyOff = false;
						}
						if(possiblyOn)
						{
							scratchPairs.push_back(std::make_pair(particleCounter, 1));
						}
						if (possiblyOff)
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

						context::inputGraph::out_edge_iterator current, last;
						boost::tie(current, last) = boost::out_edges(vertexCounter, reorderedGraph);
						for(; current != last; current++)
						{
							if((int)current->m_target < vertexCounter && copiedAlreadyPresent[particleCounter*nVertices + current->m_target])
							{
								ds.union_set(vertexCounter, (int)current->m_target);
							}
						}
					}
					else
					{
						copiedAlreadyPresent[particleCounter * nVertices + vertexCounter] = false;
					}
				}

				nActiveVertices.swap(copiedNActiveVertices);
				rank.swap(copiedRank);
				parent.swap(copiedParent);
				alreadyPresent.swap(copiedAlreadyPresent);
				anActiveVertex.swap(copiedAnActiveVertex);
				nParticles = newParticlesCount;
			}
			boost::random_shuffle(permutation, generator);
			std::string totalStr = total.str();
			totalOverPermutations += total;
		}

		args.estimate = totalOverPermutations/nPermutations;
		return true;
	}
}

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "DiscreteGermGrainObs.h"
#include "Context.h"
#include <mpir.h>
#include <mpirxx.h>
#include "isSingleComponentWithRadius.h"
#include "arguments.h"
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		//use high precision for floating point stuff
		mpf_set_default_prec(256);

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

		double probability;
		std::string message;
		if(!readProbability(variableMap, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		Context context = Context::gridContext(1, probability);
		if(!readContext(variableMap, context, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		std::size_t nVertices = boost::num_vertices(context.getGraph());
		boost::random::geometric_distribution<int, float> numberOffVertices(probability);

		mpf_class total = 0;
		mpf_class q = 1 - probability;

		std::vector<int> connectedComponents(nVertices);
		std::vector<bool> isBoundary(nVertices);
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		
		std::vector<int> allVertices;

		for(int i = 0; i < n; i++)
		{
			boost::shared_array<vertexState> state(new vertexState[nVertices]);
			std::fill(state.get(), state.get()+nVertices, vertexState::unfixed_off());
			//Pick random vertices and determine their state, until we get one that's fixed as on. 

			int nOff = numberOffVertices(randomSource);
			int componentSize = 0;
			int nBoundaryPoints = 0;
			if(nOff < (int)nVertices)
			{
				allVertices.clear();
				allVertices.reserve(nVertices);
				for(int i = 0; i < (int)nVertices; i++) allVertices.push_back(i);
				for(int i = 0; i < nOff; i++)
				{
					boost::random::uniform_int_distribution<> randomVertex(0, (int)allVertices.size()-1);
					int offVertexIndex = randomVertex(randomSource);
					state[allVertices[offVertexIndex]].state = FIXED_OFF;
					std::swap(*allVertices.rbegin(), allVertices[offVertexIndex]);
					allVertices.erase(allVertices.begin() + (allVertices.size()-1));
				}
				int onVertex;
				{
					boost::random::uniform_int_distribution<> randomVertex(0, (int)allVertices.size()-1);
					int onVertexIndex = randomVertex(randomSource);
					onVertex = allVertices[onVertexIndex];
					state[onVertex].state = FIXED_ON;
				}
				
				DiscreteGermGrainObs obs(context, state, randomSource);
				const vertexState* obsState = obs.getState();
				std::fill(connectedComponents.begin(), connectedComponents.end(), -1);
				
				std::vector<singleComponent::graphType::vertex_descriptor> specified;
				specified.push_back(onVertex);
				
				isSingleComponentAllOn(context, obsState, connectedComponents, stack);
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if(connectedComponents[k] == connectedComponents[onVertex]) componentSize++;
				}

				isBoundary.clear();
				isBoundary.resize(nVertices, false);

				Context::inputGraph::edge_iterator start, end;
				boost::tie(start, end) = boost::edges(context.getGraph());
				while(start != end)
				{
					if((obsState[start->m_source].state & ON_MASK) && (obsState[start->m_target].state & UNFIXED_OFF))
					{
						if(connectedComponents[start->m_source] == connectedComponents[onVertex])
						{
							isBoundary[start->m_target] = true;
						}
					}
					if((obsState[start->m_source].state & UNFIXED_OFF) && (obsState[start->m_target].state & ON_MASK))
					{
						if(connectedComponents[start->m_target] == connectedComponents[onVertex])
						{
							isBoundary[start->m_source] = true;
						}
					}
					start++;
				}
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if(isBoundary[k]) nBoundaryPoints++;
				}

			}
			else
			{
				nOff = (int)nVertices;
				componentSize = 0;
				nBoundaryPoints = 0;
			}
			mpf_class powerOfQ;
			int power = ((int)nVertices - nOff) - componentSize - nBoundaryPoints;
			mpf_pow_ui(powerOfQ.get_mpf_t(), q.get_mpf_t(), power); 
			total += powerOfQ;
		}
		double estimate = total.get_d() / n;
		std::cout << "Connectivity estimate is " << estimate << std::endl;
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}
